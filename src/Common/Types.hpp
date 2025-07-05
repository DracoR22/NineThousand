#pragma once

#include <string>

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
