#pragma once

#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterObject.h"
#include "../Objects/LightObject.h"
#include "../Utils/JSON.h"

namespace Scene {
	void CreateHardcodedObjects();

	std::vector<GameObject>& GetGameObjects();
	GameObject* GetGameObjectByName(const std::string& name);
	void AddGameObject(GameObjectCreateInfo createInfo);
	void RemoveGameObjectByName(const std::string& name);

	std::vector<LightObject>& GetLightObjects();
	LightObject* GetLightObjectByIndex(int index);
	void AddLightObject(LightCreateInfo& createInfo);

	std::vector<WaterObject>& GetWaterPlaneObjects();

	void LoadSceneFromFile(const std::string& filePath);
	void SaveCurrentSceneToFile();
}