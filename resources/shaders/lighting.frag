#version 430 core

#include "./common/pbr_core.glsl"

out vec4 FragColor;

#define MAX_POINT_LIGHTS 20
struct Light {
    float posX;
    float posY;
    float posZ;
    float radius;

    float strength;
    float colorR;
    float colorG;
    float colorB;

    float type;
    float _padding0;
    float _padding1;
    float _padding2;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPos;
in mat3 TBN;

layout (std430, binding = 0) readonly buffer LightsBuffer {
    Light lights[MAX_POINT_LIGHTS];
};

layout (std430, binding = 1) readonly buffer LightSpaceMatricesBuffer {
    mat4 lightSpaceMatrices[];
};

uniform mat4 view;
uniform int numLights;
uniform vec3 camPos;

uniform sampler2DArray shadowMap;
uniform float farPlane;

uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
uniform vec3 lightDir;


float ShadowCalculationCSM(vec3 fragPosWorldSpace) {
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    vec4 csmClipSpaceZFar = vec4(cascadePlaneDistances[0], cascadePlaneDistances[1], cascadePlaneDistances[2], cascadePlaneDistances[3]);
    vec4 cmp = step(csmClipSpaceZFar, vec4(depthValue)); 
    int layer = int(cmp.x + cmp.y + cmp.z + cmp.w);

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    if (currentDepth > 1.0) {
        return 0.0;
    }
  
    vec3 normal = normalize(Normal);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.05f;
    if (layer == cascadeCount) {
        bias *= 1.0 / (farPlane * biasModifier);
    }
    else {
        bias *= 1.0 / (cascadePlaneDistances[layer] * biasModifier);
    }

  // PCF 2x2 centered
  float shadow = 0.0;
  vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
  vec2 offsets[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5)
  );

  for (int i = 0; i < 4; ++i) {
    float pcfDepth = texture(
        shadowMap,
        vec3(projCoords.xy + offsets[i] * texelSize, layer)
    ).r;
    shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
   }

   shadow /= 4.0;
    
   // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
   if(projCoords.z > 1.0) {
       shadow = 0.0;
   }
    
   return shadow * 0.4;
}


vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalTexture, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}


void main() {
 vec3 albedo = pow(texture(baseTexture, TexCoords).rgb, vec3(2.2));
 vec3 rma = texture(rmaTexture, TexCoords).rgb;

 float roughness = rma.r;
 float metallic  = rma.g;
 float ao        = rma.b;

 vec3 N = getNormalFromMap();
 vec3 V = normalize(camPos - WorldPos);

 vec3 color = vec3(0.0);

 for (int i = 0; i < numLights; ++i) {
    Light light = lights[i];
    vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
    vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);

    float attenuation = 1.0;

    vec3 L;
    vec3 radiance;

  if (int(light.type) == 0) { // Point light
     L = normalize(lightPosition - WorldPos);

     float distance    = length(lightPosition - WorldPos);
     attenuation =  smoothstep(light.radius, 0, length(lightPosition - WorldPos));    
     radiance = lightColor * attenuation * light.strength;
  } else { // Directional light
     L = normalize(-vec3(1.0, -1.0, 0.0)); 

     radiance = lightColor * light.strength;
  }

   color += MicrofacetBRDF(L, V, N, albedo, metallic, 1.0, roughness) * radiance;
 }

  float shadow = ShadowCalculationCSM(WorldPos);
  color *= (1.0 - shadow);

  vec3 ambient = vec3(0.02) * albedo * ao;

  FragColor = vec4(color, 1.0);
}