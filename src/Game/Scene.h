#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterObject.h"
#include "../Utils/JSON.h"

namespace Scene {
	void CreateHardcodedObjects();
	void AddGameObject(GameObjectCreateInfo createInfo);
	void RemoveGameObjectByName(const std::string& name);

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);

	std::vector<WaterObject>& GetWaterPlaneObjects();

	void LoadSceneFromFile(const std::string& filePath);
	void SaveCurrentSceneToFile();
}