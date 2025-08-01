#include "Scene.h"

namespace Scene {
	std::vector<GameObject> g_gameObjects;
	std::vector<GameObject> g_animatedObjects;
	std::vector<WaterObject> g_waterPlaneObjects;

	void CreateHardcodedObjects() {
		WaterObjectCreateInfo waterPlaneCreateInfo{
			"WaterPlane0",
			"Plane",
			glm::vec3(-4.0f, 0.5f, 8.0f),
			glm::vec3(100.0f),
			glm::mat4(1.0f),
			glm::vec3(0.0, 0.3, 0.5),
			0.5f
		};

		g_waterPlaneObjects.emplace_back(waterPlaneCreateInfo);
	}

	void AddGameObject(GameObjectCreateInfo createInfo) {
		g_gameObjects.emplace_back(createInfo);
	}

	void RemoveGameObjectByName(const std::string& name) {
		g_gameObjects.erase(
			std::remove_if(g_gameObjects.begin(), g_gameObjects.end(),
				[&](const GameObject& obj) {
					return obj.GetName() == name;
				}),
			g_gameObjects.end()
		);
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

	std::vector<WaterObject>& GetWaterPlaneObjects() {
		return g_waterPlaneObjects;
	}

	void LoadSceneFromFile(const std::string& filePath) {
		LevelCreateInfo levelCreateInfo = JSON::LoadLevel(filePath);

		g_gameObjects.clear();
		
		for (GameObjectCreateInfo& createInfo : levelCreateInfo.gameObjects) {
			g_gameObjects.emplace_back(createInfo);
		}
	}
}