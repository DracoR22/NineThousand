#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Graphics/Primitives.hpp"

struct PointLight {
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};


namespace Scene {
    inline std::vector<PrimitiveModel> g_primitiveGameObjects;
	inline std::vector<PointLight> g_scenePointLights;

	void LoadGamePrimitiveObjects();

	PrimitiveModel* GetPrimitiveModelByName(const std::string& name);
}