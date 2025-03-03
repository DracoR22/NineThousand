#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in ivec4 aBoneIds;
layout(location = 5) in vec4 aWeights;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLight;
out mat3 TBN;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 lightProjection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
  vec4 totalPosition = vec4(0.0f);
  vec3 totalNormal = vec3(0.0f);
  vec3 totalTangent = vec3(0.0);

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIds[i] == -1) 
            continue;
        if(aBoneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            totalNormal = aNorm;
            totalTangent = aTangent;
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * aWeights[i];

        vec3 localNormal = mat3(finalBonesMatrices[aBoneIds[i]]) * aNorm;
        totalNormal += localNormal * aWeights[i];

        vec3 localTangent = mat3(finalBonesMatrices[aBoneIds[i]]) * aTangent;
        totalTangent += localTangent * aWeights[i];
   }

    FragPos = vec3(model * totalPosition);
    Normal = normalize(totalNormal);
    TexCoords = aTex;

    FragPosLight = lightProjection * vec4(FragPos, 1.0);

    vec3 N = Normal;
    vec3 T = normalize(totalTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);

    mat4 viewModel = view * model;
    gl_Position = projection * viewModel * totalPosition;
}