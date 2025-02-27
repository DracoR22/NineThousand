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

	void LoadSceneLights() {
		/*g_scenePointLights.clear();

		PointLight& mainPointLight = g_scenePointLights.emplace_back();
		mainPointLight.position = AssetManager::GetModelByName("CubeLamp")->pos;
		mainPointLight.constant = 1.0f;
		mainPointLight.linear = 0.009f;
		mainPointLight.quadratic = 0.01f;
		mainPointLight.ambient = glm::vec3(1.3f);
		mainPointLight.diffuse = glm::vec3(0.8f);
		mainPointLight.specular = glm::vec3(1.0f);*/
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