#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Common/Types.hpp"

enum class LightType {
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT = 1
};

struct GameObjectCreateInfo {
	std::string name = "undefined";
	std::string modelName = "undefined";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec2 textureScale = glm::vec2(1.0f);
	std::vector<MeshRenderingInfo> meshRenderingInfo;
};

struct BulletCaseCreateInfo {
	std::string modelName = "undefined";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	int materialIndex = 0;
	int physicsId = 0;
};

struct WaterObjectCreateInfo {
	std::string name = "undefined";
	std::string modelName = "undefined";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::vec3 waterColor = glm::vec3(0.0f);
	float waterTransparency = 0.0f;
};

struct LightCreateInfo {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(0.8f);

	float radius = 10.0f;
	float strength = 1.0f;

	LightType type = LightType::POINT_LIGHT;
};

struct LevelCreateInfo {
	std::string name;
	std::vector<GameObjectCreateInfo> gameObjects;
	std::vector<LightCreateInfo> lights;
};