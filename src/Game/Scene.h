#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterObject.h"
#include "../Objects/LightObject.h"
#include "../Objects/BulletCaseObject.h"
#include "../Objects/PickUpObject.h"
#include "../Utils/JSON.h"
#include "../Game/Mannequin.h"
#include "../Objects/BloodSplatterObject.h"

namespace Scene {
	void CreateHardcodedObjects();

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);
	void AddGameObject(GameObjectCreateInfo createInfo);
	void RemoveGameObjectByName(const std::string& name);

	std::vector<LightObject>& GetLightObjects();
	LightObject* GetLightObjectByIndex(int index);
	void AddLightObject(LightCreateInfo& createInfo);
	void RemoveLightObjectByIndex(int index);

	std::vector<WaterObject>& GetWaterPlaneObjects();

	std::vector<BulletCaseObject>& GetBulletCaseObjects();
	void AddBulletCaseObject(BulletCaseCreateInfo& createInfo);
	void RemoveBulletCaseObjectByIndex(int index);

	std::vector<PickUpObject>& GetPickUpObjects();

	void AddMannequin(glm::vec3 position);
	Mannequin* GetMannequinById(uint64_t mannequinId);
	std::vector<Mannequin>& GetAllMannequins();

	void AddBloodSplatterObject(glm::vec3 position, glm::vec3 forward);
	std::vector<BloodSplatterObject>& GetBloodSplatterObjects();

	void LoadSceneFromFile(const std::string& filePath);
	void SaveCurrentSceneToFile();
}