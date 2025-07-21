#version 430 core

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

    int type;
    float _padding0;
    float _padding1;
    float _padding2;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPos;
in mat3 TBN;

uniform mat4 view;

uniform Light lights[MAX_POINT_LIGHTS];
uniform int noLights;
uniform vec3 camPos;

uniform sampler2DArray shadowMap;
uniform float farPlane;
uniform bool castShadows;

uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;
uniform vec3 lightDir;

const float PI = 3.14159265359;

vec3 gridSamplingDiskCSM[20] = vec3[](
    vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
    vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
    vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
    vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

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

    if (currentDepth > 1.0)
    {
        return 0.0;
    }
  
    vec3 normal = normalize(Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount) {
        bias *= 1.0 / (farPlane * biasModifier);
    }
    else {
        bias *= 1.0 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    vec3 baseCoords = vec3(projCoords.xy, layer);
    float testDepth = currentDepth - bias;

    // Reduce quality for distant shadows
    int sampleRadius = (layer < 2) ? 1 : 0; // Only high quality for close cascades
    int sampleCount = 0;

    for(int x = -sampleRadius; x <= sampleRadius; ++x) {
    for(int y = -sampleRadius; y <= sampleRadius; ++y) {
        float pcfDepth = texture(shadowMap, baseCoords + vec3(vec2(x, y) * texelSize, 0.0)).r;
        shadow += testDepth > pcfDepth ? 1.0 : 0.0;
        sampleCount++;
      }    
    }
    shadow /= float(sampleCount);

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

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main() {
 vec3 albedo = pow(texture(baseTexture, TexCoords).rgb, vec3(2.2));
 vec3 rma = texture(rmaTexture, TexCoords).rgb;
 float roughness = rma.r;
 float metallic  = rma.g;
 float ao        = rma.b;

 vec3 N = getNormalFromMap();

 vec3 V = normalize(camPos - WorldPos);

 vec3 Lo = vec3(0.0);

 vec3 F0 = vec3(0.04); 
 F0  = mix(F0, albedo, metallic);

 for (int i = 0; i < noLights; ++i) {
    Light light = lights[i];
    vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
    vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);

    vec3 L;
    float attenuation = 1.0;
    vec3 radiance;

  if (light.type == 0) { // Point light
     L = normalize(lightPosition - WorldPos);

     float distance    = length(lightPosition - WorldPos);
     attenuation =  smoothstep(light.radius, 0, length(lightPosition - WorldPos));    
     radiance = lightColor * attenuation * light.strength;
  } else { // Directional light
     L = normalize(-vec3(1.0, -1.0, 0.0)); 

     radiance = lightColor * light.strength;
  }

     vec3 H = normalize(V + L);

   vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
   float NDF = DistributionGGX(N, H, roughness);       
   float G   = GeometrySmith(N, V, L, roughness);    

   vec3 numerator    = NDF * G * F;
   float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
   vec3 specular     = numerator / denominator;  

   vec3 kS = F;
   vec3 kD = vec3(1.0) - kS;
  
   kD *= 1.0 - metallic;

   float NdotL = max(dot(N, L), 0.0);        
   Lo += (kD * albedo / PI + specular) * radiance * NdotL;
 }

  if (castShadows) {
   float shadow = ShadowCalculationCSM(WorldPos);
   Lo *= (1.0 - shadow);
  }

  vec3 ambient = vec3(0.02) * albedo * ao;
  //vec3 color   = ambient + Lo; 
  vec3 color = Lo;

  FragColor = vec4(color, 1.0);
}