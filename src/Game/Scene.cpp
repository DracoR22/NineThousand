#include "Scene.h"

namespace Scene {
	void LoadGamePrimitiveObjects() {
		g_primitiveGameObjects.emplace_back("Cube", PrimitiveModel::Type::CUBE, glm::vec3(0.0f, 5.0f, 1.0f), glm::vec3(0.75f));
		g_primitiveGameObjects.back().Init();

		g_primitiveGameObjects.emplace_back("Plane", PrimitiveModel::Type::PLANE, glm::vec3(0.0f), glm::vec3(50.0f));
		g_primitiveGameObjects.back().Init();

		g_primitiveGameObjects.emplace_back("CubeLamp", PrimitiveModel::Type::CUBE, glm::vec3(5.0f, 5.0f, 5.0f));
		g_primitiveGameObjects.back().Init();

		std::cout << "All Primitives Loaded" << std::endl;
	}

	PrimitiveModel* GetPrimitiveModelByName(const std::string& name) {
		for (auto& primitiveGameObject : g_primitiveGameObjects) {
			if (primitiveGameObject.GetName() == name) {
				return &primitiveGameObject;
			}
		}
		std::cout << "No Model found: " << name << std::endl;
		return nullptr;
	}
}