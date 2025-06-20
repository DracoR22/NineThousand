#include "Scene.h"

namespace Scene {
	std::vector<GameObject> g_gameObjects;
	std::vector<GameObject> g_animatedObjects;
	std::vector<WaterObject> g_waterPlaneObjects;

	void CreateHardcodedObjects() {
		// harcoded game objects
		GameObjectCreateInfo cubeCreateInfo{
			"Cube0",
			"Cube",
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::vec3(0.0f),
		};

		GameObjectCreateInfo planeCreateInfo{
			"Plane0",
			"Plane",
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::vec3(0.0f),
		};

		GameObjectCreateInfo plane1CreateInfo{
			"Plane1",
			"Plane",
			glm::vec3(0.0f, 10.0f, 0.0f),
			glm::vec3(10.0f),
			glm::vec3(0.0f),
		};

		g_gameObjects.clear();

		g_gameObjects.emplace_back(cubeCreateInfo);
		g_gameObjects.emplace_back(planeCreateInfo);
		g_gameObjects.emplace_back(plane1CreateInfo);
	}

	void AddGameObject(GameObjectCreateInfo createInfo) {
		g_gameObjects.emplace_back(createInfo);
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

	void CreateWaterPlaneObjects() {
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