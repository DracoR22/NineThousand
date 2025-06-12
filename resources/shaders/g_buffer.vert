#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 WorldPos;
out mat3 TBN;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
  
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}