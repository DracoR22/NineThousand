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
in vec3 Normal;
in vec3 WorldPos;
in vec4 WorldPosLight;
in mat3 TBN;

uniform Light lights[MAX_POINT_LIGHTS];
uniform int noLights;

uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

uniform vec3 camPos;

uniform bool flipLights;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalTexture, TexCoords).xyz * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
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


// TONE MAPPING

vec3 ACESFilm(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 filmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

vec3 reinhard(vec3 x) {
  return x / (1.0 + x);
}

vec3 uncharted2Tonemap(vec3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  float W = 11.2;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(vec3 color) {
  const float W = 11.2;
  float exposureBias = 2.0;
  vec3 curr = uncharted2Tonemap(exposureBias * color);
  vec3 whiteScale = 1.0 / uncharted2Tonemap(vec3(W));
  return curr * whiteScale;
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
     L = normalize(-vec3(0.0, -1.0, 0.0)); 

     radiance = lightColor * light.strength;
  }

   vec3 H = normalize(V + L);

    vec3 F  = fresnelSchlickRoughness(max(dot(H, V), 0.0), F0, roughness);
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

 vec3 ambient = vec3(0.03) * albedo * ao;
 vec3 color   =  Lo;  

  //color = uncharted2(color);
  //color = pow(color, vec3(1.0/2.2));

 FragColor = vec4(color, 1.0);
}