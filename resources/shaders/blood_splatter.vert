#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 inverseModel;
uniform mat4 view;
uniform mat4 projection;
uniform float u_time;

uniform sampler2D u_PosTex;
uniform sampler2D u_NormTex;

void main() {
  int u_NumOfFrames = 81;
  int u_Speed = 35;
  vec3 u_HeightOffset = vec3(-45.4, -26.17, 12.7);

  u_HeightOffset = vec3(0, 0, 0);

  float currentSpeed = 1.0f / (u_NumOfFrames / u_Speed);
  float timeInFrames = ((ceil(fract(-u_time * currentSpeed) * u_NumOfFrames)) / u_NumOfFrames) + (1.0 / u_NumOfFrames);

  timeInFrames = 0.0;
  timeInFrames = u_time;

  vec2 TexCoord = vec2(aTexCoords.x, (timeInFrames + aTexCoords.y));
  TexCoord = clamp(TexCoord, 0, 1);

  vec4 texturePos = textureLod(u_PosTex, TexCoord, 0);
  vec4 textureNorm = textureLod(u_NormTex, TexCoord, 0);

  vec3 normal = textureNorm.xzy * 2.0 - 1.0;
  mat3 normalMatrix = transpose(mat3(inverseModel));
  Normal = normalize(normalMatrix * normal);

  WorldPos = model * vec4(texturePos.xzy, 1.0);

  gl_Position = projection * view * WorldPos;

   //if (u_time > 0.7) {
    //    gl_Position =  vec4(0);
   //}
}