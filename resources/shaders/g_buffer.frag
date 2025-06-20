#version 330 core

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 BaseColorOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 RMAOut;

in vec2 TexCoords;
in vec3 Normal;
in vec3 WorldPos;
in mat3 TBN;

uniform sampler2D baseTexture;
uniform sampler2D normalTexture;
uniform sampler2D rmaTexture;

void main() {
  WorldPosOut = vec4(WorldPos.rgb, 1.0);
  BaseColorOut = texture(baseTexture, TexCoords);

  vec3 tangentNormal = texture(normalTexture, TexCoords).xyz * 2.0 - 1.0;
  NormalOut = vec4(normalize(TBN * tangentNormal), 1.0);

  RMAOut = texture(rmaTexture, TexCoords);
}