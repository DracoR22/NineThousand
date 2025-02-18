#include "Scene.h"

namespace Scene {
	void LoadGamePrimitiveObjects() {
		ModelCreateInfo cubeCreateInfo{
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo lampCreateInfo{
			glm::vec3(10.0f, 5.0f, 5.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo planeCreateInfo{
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::mat4(1.0f)
		};
	
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