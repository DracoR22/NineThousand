#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterPlaneObject.h"

namespace Scene {
	void CreateGameObjects();
	void AddGameObject(GameObjectCreateInfo createInfo);

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);

	void CreateWaterPlaneObjects();
	std::vector<WaterPlaneObject>& GetWaterPlaneObjects();

	void LoadSceneFromFile();
	void SaveCurrentSceneToFile();
}