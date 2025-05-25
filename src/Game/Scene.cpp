#include "Scene.h"

namespace Scene {
	std::vector<GameObject> g_gameObjects;
	std::vector<GameObject> g_animatedObjects;

	void CreateGameObjects() {
		// harcoded game objects
		GameObjectCreateInfo cubeCreateInfo{
			"Cube0",
			"Cube",
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f),
		};

		GameObjectCreateInfo planeCreateInfo{
			"Plane0",
			"Plane",
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::mat4(1.0f),
		};

		g_gameObjects.clear();

		g_gameObjects.emplace_back(cubeCreateInfo);
		g_gameObjects.emplace_back(planeCreateInfo);
	}

	std::vector<GameObject>& GetGameObjects() {
		return g_gameObjects;
	}

	GameObject* GetGameObjectByName(const std::string& name) {
		for (GameObject& gameObject : g_gameObjects) {
			if (gameObject.GetName() == name) {
				return &gameObject;
			}
	     }

		std::cout << "Scene::GetGameObjectByName() failed because " << name << "does not exist!\n";
		return nullptr;
	}
}