#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 WorldPos;
out vec2 TexCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main() {
  WorldPos = vec3(modelMatrix * vec4(aPos, 1.0));
  TexCoords = aTexCoord;

  gl_Position = projectionMatrix * viewMatrix * vec4(WorldPos, 1.0);
}