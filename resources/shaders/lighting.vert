#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;
out vec4 WorldPosLight;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform mat4 lightProjection;
uniform vec2 textureScale;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoord * textureScale;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    WorldPosLight = lightProjection * vec4(WorldPos, 1.0);

    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
  
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}