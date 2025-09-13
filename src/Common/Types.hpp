#pragma once

#include <string>

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