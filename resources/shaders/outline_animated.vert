#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform float outlineThickness;

void main() {
    // scale around normal
    vec3 viewPos = vec3(view * model * vec4(aPos, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    vec3 viewNormal = normalize(normalMatrix * aNormal);

    vec3 inflatedViewPos = viewPos + viewNormal * 0.02;

    gl_Position = projection * vec4(inflatedViewPos, 1.0);

    //gl_Position = projection * view * model * vec4(aPos, 1.0);
}