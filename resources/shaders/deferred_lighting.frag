#version 330 core

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

//in vec4 WorldPosLight;

uniform Light lights[MAX_POINT_LIGHTS];
uniform int noLights;
uniform vec3 camPos;

uniform sampler2D shadowMap;
uniform samplerCube shadowCubemap;

uniform float farPlane;

uniform sampler2D gPosition;
uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

const float PI = 3.14159265359;



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

float ShadowCalculationCubeMap(vec3 worldPos, Light light) {
   vec3 lightPos = vec3(light.posX, light.posY, light.posZ);
    vec3 fragToLight = worldPos - lightPos;

    float currentDepth = length(fragToLight);
    float bias = 0.05; 

    float closestDepth = texture(shadowCubemap, fragToLight).r;
    closestDepth *= farPlane;

    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, Light light, vec3 worldPos)
{
   vec3 lightDir = normalize(vec3(light.posX, light.posY, light.posZ) - worldPos);
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) {
      return 0.0;
    }

   float closestDepth = texture(shadowMap, projCoords.xy).r; 
   float currentDepth = projCoords.z;

   float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // correct shadow color
    shadow *= 0.8;

    return shadow;
}  

void main() {
 vec3 WorldPos = texture(gPosition, TexCoords).rgb;
 vec3 albedo = pow(texture(baseTexture, TexCoords).rgb, vec3(2.2));
 vec3 rma = texture(rmaTexture, TexCoords).rgb;
 float roughness = rma.r;
 float metallic  = rma.g;
 float ao        = rma.b;

 vec3 N = texture(normalTexture, TexCoords).rgb;
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

   // calculate shadows
   vec4 WorldPosLight = vec4(1.0);
   float shadow = ShadowCalculation(WorldPosLight, N, lights[i], WorldPos);
   //float shadow = ShadowCalculationPointLight(WorldPos, lights[i]);
   Lo *= (1.0 - shadow);
 }

 vec3 ambient = vec3(0.02) * albedo * ao;
 //vec3 color   = ambient + Lo; 
 vec3 color =  Lo;

  //color = uncharted2(color);
  //color = pow(color, vec3(1.0/2.2));

  FragColor = vec4(color, 1.0);
}