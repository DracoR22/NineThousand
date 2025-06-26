#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform vec2 textureScale;

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoord * textureScale;

    //mat3 normalMatrix = transpose(inverse(mat3(model)));

    Normal = normalize(normalMatrix * aNormal);
    vec3 T = normalize(mat3(model) * aTangent);   
    T = normalize(T - dot(T, Normal) * Normal);
    vec3 B = cross(Normal, T);

    TBN = mat3(T, B, Normal);
  
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}
