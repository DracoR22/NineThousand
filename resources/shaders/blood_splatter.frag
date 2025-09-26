#version 430 core

out vec4 FragColor;

in vec4 WorldPos;
in vec3 Normal;

vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));

void main() {
  vec3 N = normalize(Normal);
  vec3 L = normalize(lightDir);

  float diff = max(dot(N, L), 0.0);

   vec3 baseColor = vec3(0.1, 0.0, 0.0);

   vec3 ambient = 0.2 * baseColor;

   vec3 finalColor = ambient + diff * baseColor;

  FragColor = vec4(finalColor, 1.0);
}