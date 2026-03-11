#version 430 core

#include "./common/lights.glsl"

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

in vec4 WorldPos;
in vec3 Normal;

layout (std430, binding = 0) readonly buffer LightsBuffer {
    Light lights[MAX_POINT_LIGHTS];
};

uniform int numLights;
uniform vec3 camPos;

void main() {
  vec3 baseColor = vec3(0.5, 0.0, 0.0);
  vec3 normalColor = vec3(normalize(Normal));
  vec3 rmaColor = vec3(0.015 , 0.54, 1.0);

  float roughness = rmaColor.r;
  float metallic  = rmaColor.g;
  float ao        = rmaColor.b;

  vec3 finalColor = vec3(0.0);

   for (int i = 0; i < numLights; ++i) {
    Light light = lights[i];

    vec3 lightPos = vec3(light.posX, light.posY, light.posZ);
    vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);

    if (int(light.type) == 0) {
      finalColor += GetSpotLightLighting(lightPos, lightColor, light.strength, light.radius, WorldPos.xyz, camPos, Normal, baseColor, metallic, roughness);
    } else {
      vec3 lightDir = -vec3(1.0, -1.0, 0.0);
      finalColor += GetDirectionalLighting(lightDir, lightColor, light.strength, WorldPos.xyz, camPos, Normal, baseColor, metallic, roughness);
    }
 }

  FragColor = vec4(finalColor, 1.0);
}