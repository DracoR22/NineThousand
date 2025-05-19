#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType {
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT = 1
};

struct ModelCreateInfo {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	std::string baseTexture = "Concrete_ALB.png";
	std::string normalTexture = "Concrete_NRM.png";
	std::string rmaTexture = "Concrete_RMA.png";
	std::vector<glm::vec3> instanceOffsets = {};
};

struct LightCreateInfo {
	glm::vec3 position = glm::vec3(0.0f);

	float constant = 1.0f;
	float linear = 1.0f;
	float quadratic = 1.0f;
	float radius = 10.0f;
	float strength = 1.0f;

	glm::vec3 ambient = glm::vec3(0.0f);
	glm::vec3 diffuse = glm::vec3(0.0f);
	glm::vec3 specular = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.8f);

	LightType type = LightType::POINT_LIGHT;
};