#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"

namespace Scene {
	void CreateGameObjects();
	void LoadSceneFromFile();

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);
}