#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class LightType {
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT = 1
};

enum class UIAlignment {
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	Center,
	TopCenter,
	BottomCenter
};

struct GameObjectCreateInfo {
	std::string name = "undefined";
	std::string modelName = "undefined";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 size = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec2 textureScale = glm::vec2(1.0f);
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

struct ModelCreateInfo {
	std::string baseTexture = "Concrete_ALB.png";
	std::string normalTexture = "Concrete_NRM.png";
	std::string rmaTexture = "Concrete_RMA.png";
	std::vector<glm::vec3> instanceOffsets = {};
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

struct UIElementCreateInfo {
	UIAlignment aligment = UIAlignment::TopLeft;
	bool useAligment = false;
	bool isVisible = false;

	glm::vec2 size = glm::vec2(1.0f);
	glm::vec2 positionY = glm::vec2(1.0f);
	glm::vec2 positionX = glm::vec2(1.0f);

	glm::vec3 color = glm::vec3(1.0f);
	std::string textureName = "undefined";
};