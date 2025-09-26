#pragma once

#include <string>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>

#define MAX_LIGHTS 20

struct Material {
	std::string name = "";
	int baseTexture = 0;
	int normalTexture = 0;
	int rmaTexture = 0;
};

struct MeshRenderingInfo {
	uint32_t meshIndex;
	uint32_t materialIndex;
};

struct GPULight {
	float posX;
	float posY;
	float posZ;
	float radius;
	float strength;
	float colorR;
	float colorG;
	float colorB;
	float type;
	float _padding0;
	float _padding1;
	float _padding2;
};

struct PhysicsTransformData {
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};