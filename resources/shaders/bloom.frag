#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 lightColor;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	BrightColor = vec4(lightColor, 1.0);
}