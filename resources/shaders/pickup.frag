#version 430 core

layout (location = 0) out vec4 FragColor;

in vec3 WorldPos;
in vec2 TexCoords;

uniform float time;
uniform sampler2D baseTexture;

void main() {
  vec3 flashColor = vec3(1.0, 1.0, 1.0);
  float flashSpeed = 3.5; // cycles per second
  float flashIntensity = smoothstep(0.2, 1.0, sin(time * flashSpeed) * 0.5 + 0.5);

  vec3 albedo = texture(baseTexture, TexCoords).rgb; 

  vec3 finalColor = mix(albedo, flashColor, flashIntensity * 0.3);
  FragColor = vec4(finalColor, 1.0);
}