#version 430 core

#include "./common/lights.glsl"

layout (location = 0) out vec4 FragColor;

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

in vec3 WorldPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

layout (std430, binding = 0) readonly buffer LightsBuffer {
    Light lights[MAX_POINT_LIGHTS];
};

uniform int numLights;
uniform vec3 viewPos;
uniform float time;
uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

void main() {
  vec3 flashColor = vec3(1.0, 1.0, 1.0);
  float flashSpeed = 3.5;
  float flashIntensity = smoothstep(0.2, 1.0, sin(time * flashSpeed) * 0.5 + 0.5);

  vec3 albedo = pow(texture(baseTexture, TexCoords).rgb, vec3(2.2));
  vec3 N = normalize(TBN * (texture(normalTexture, TexCoords).xyz * 2.0 - 1.0));
  vec3 rma = texture(rmaTexture, TexCoords).rgb;

  float roughness = rma.r;
  float metallic  = rma.g;
  float ao        = rma.b;

  vec3 brdf = vec3(0.0);

  for (int i = 0; i < numLights; i++) {
    Light light = lights[i];
    vec3 lightPos = vec3(light.posX, light.posY, light.posZ);
    vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);

    if (int(light.type) == 0) {
       brdf += GetSpotLightLighting(lightPos, lightColor, light.strength, light.radius, WorldPos, viewPos, N, albedo, metallic, roughness);
    }
  }

  vec3 finalColor = brdf + flashColor * flashIntensity * 0.3;

  FragColor = vec4(finalColor, 1.0);
}