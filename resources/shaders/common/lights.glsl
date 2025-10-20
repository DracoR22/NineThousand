#include "./pbr_core.glsl"

vec3 GetSpotLightLighting(vec3 lightPos, vec3 lightColor, float lightStrength, float lightRadius, vec3 WorldPos, vec3 viewPos, vec3 Normal, vec3 baseColor, float metallic, float roughness) {
   vec3 L = normalize(lightPos - WorldPos);
   vec3 V = normalize(viewPos - WorldPos);

   float distance = length(lightPos - WorldPos);
   float attenuation = smoothstep(lightRadius, 0, distance);    
   vec3 radiance = lightColor * attenuation * lightStrength;

   vec3 brdf = MicrofacetBRDF(L, V, Normal, baseColor, metallic, 1.0, roughness) * radiance;

   return brdf;
}

vec3 GetDirectionalLighting(vec3 lightDir, vec3 lightColor, float lightStrength, vec3 WorldPos, vec3 viewPos, vec3 Normal, vec3 baseColor, float metallic, float roughness) {
   vec3 L = normalize(lightDir); 
   vec3 V = normalize(viewPos - WorldPos);

   vec3 radiance = lightColor * lightStrength;

   vec3 brdf = MicrofacetBRDF(L, V, Normal, baseColor, metallic, 1.0, roughness) * radiance;

   return brdf;
}