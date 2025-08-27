#version 330 core

out vec4 FragColor;

uniform float time;

vec3 originalColor = vec3(0.0, 1.0, 0.0);
vec3 flashColor = vec3(0.0, 1.0, 0.9);

void main() {
   float flashIntensity = sin(time * 4.0) * 0.5 + 0.5; 
   vec3 finalColor = mix(originalColor, flashColor, flashIntensity);
   FragColor = vec4(finalColor, 1.0);
}