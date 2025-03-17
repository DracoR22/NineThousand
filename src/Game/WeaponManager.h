#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Physics/Physics.h"

struct BulletInfo {
	Model* model;
	physx::PxRigidDynamic* actor;
};

struct WeaponAnimations {
	std::string idle;
	std::string walk;
	std::string reload;
	std::string reloadEmpty;
	std::string draw;
	std::vector<std::string> fire;
};

struct WeaponInfo {
	std::string name;
	std::string modelName;
	WeaponAnimations animations;
};

namespace WeaponManager {
	void Init();
	WeaponInfo* GetWeaponInfoByName(std::string name);
}