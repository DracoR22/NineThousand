#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterPlaneObject.h"

namespace Scene {
	void CreateGameObjects();
	void CreateWaterPlaneObjects();

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);

	std::vector<WaterPlaneObject>& GetWaterPlaneObjects();

	void LoadSceneFromFile();
}