#pragma once

#include <glm/glm.hpp>

struct Light {
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	float k0;
	float k1;
	float k2;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};