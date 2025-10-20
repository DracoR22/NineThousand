#include "Scene.h"

namespace Scene {
	std::vector<GameObject> g_gameObjects;
	std::vector<WaterObject> g_waterPlaneObjects;
	std::vector<LightObject> g_lightObjects;
	std::vector<BulletCaseObject> g_bulletCaseObjects;
	std::vector<BloodSplatterObject> g_bloodSplatterObjects;
	std::vector<PickUpObject> g_pickUpObjects;

	std::vector<Mannequin> g_mannequins;

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

		PickUpObjectCreateInfo pickUpObjectCreateInfo{
			glm::vec3(5.0f, 3.0f, 5.0f),
			glm::vec3(5.0f),
			glm::vec3(0.0f),
			PickUpType::AKS74U,
			{}
		};

		g_pickUpObjects.emplace_back(pickUpObjectCreateInfo);
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

	std::vector<LightObject>& GetLightObjects() {
		return g_lightObjects;
	}

	LightObject* GetLightObjectByIndex(int index) {
		if (index < 0 || index >= (int)g_lightObjects.size()) {
			std::cout << "Scene::GetLightObjectByIndex() failed because index " << index << "is out of range!\n";
			return nullptr;
		}

		for (int i = 0; i < g_lightObjects.size(); i++) {
			if (i == index) {
				return &g_lightObjects[i];
			}
		}

		std::cout << "Scene::GetLightObjectByIndex() failed because " << "could not get light by index!\n";
		return nullptr;
	}

	void AddLightObject(LightCreateInfo& createInfo) {
		g_lightObjects.emplace_back(createInfo);
	}

	void RemoveLightObjectByIndex(int index) {
		if (index < g_lightObjects.size()) {
			g_lightObjects.erase(g_lightObjects.begin() + index);
		}
	}

	std::vector<WaterObject>& GetWaterPlaneObjects() {
		return g_waterPlaneObjects;
	}

	std::vector<BulletCaseObject>& GetBulletCaseObjects() {
		return g_bulletCaseObjects;
	}
	void AddBulletCaseObject(BulletCaseCreateInfo& createInfo) {
		g_bulletCaseObjects.emplace_back(createInfo);
	}

	void RemoveBulletCaseObjectByIndex(int index) {
		if (index < g_bulletCaseObjects.size()) {
			Physics::MarkRigidDynamicForRemoval(g_bulletCaseObjects[index].GetPhysicsId());
			g_bulletCaseObjects.erase(g_bulletCaseObjects.begin() + index);
		}
	}

	std::vector<PickUpObject>& GetPickUpObjects() {
		return g_pickUpObjects;
	}

	void AddMannequin(glm::vec3 position) {
		g_mannequins.emplace_back(position);
	}

	Mannequin* GetMannequinById(uint64_t mannequinId) {
		for (Mannequin& mannequin : g_mannequins) {
			if (mannequin.GetObjectId() == mannequinId) {
				return &mannequin;
			}
		}

		return nullptr;
	}

	std::vector<Mannequin>& GetAllMannequins() {
		return g_mannequins;
	}

	void AddBloodSplatterObject(glm::vec3 position, glm::vec3 forward) {
		int maxAllowed = 4;
		if (g_bloodSplatterObjects.size() < maxAllowed) {
			g_bloodSplatterObjects.push_back(BloodSplatterObject(position, forward));
		}
	}

	std::vector<BloodSplatterObject>& GetBloodSplatterObjects() {
		return g_bloodSplatterObjects;
	}

	void LoadSceneFromFile(const std::string& filePath) {
		LevelCreateInfo levelCreateInfo = JSON::LoadLevel(filePath);

		g_gameObjects.clear();
		g_lightObjects.clear();
		
		for (GameObjectCreateInfo& createInfo : levelCreateInfo.gameObjects) {
			g_gameObjects.emplace_back(createInfo);
		}

		for (LightCreateInfo& createInfo : levelCreateInfo.lights) {
			g_lightObjects.emplace_back(createInfo);
		}
	}
}