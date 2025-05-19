#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 WorldPos;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

const vec4 clipPlane = vec4(0, -1, 0, 15);

void main() {
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoord;
  
    gl_ClipDistance[0] = dot(vec4(WorldPos, 1.0), clipPlane);
    gl_Position = projection * view * vec4(WorldPos, 1.0);
}