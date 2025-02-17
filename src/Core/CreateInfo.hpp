#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ModelCreateInfo {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
};