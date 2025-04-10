#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Physics/Physics.h"

enum class WeaponAction {
	IDLE,
	DRAW,
	ADS_IN,
	ADS_OUT,
	ADS_IDLE,
	ADS_FIRE,
};

struct WeaponAnimations {
	std::string idle;
	std::string walk;
	std::string reload;
	std::string reloadEmpty;
	std::string draw;
	std::vector<std::string> fire;
	std::string ADSIdle;
	std::string ADSIn;
	std::string ADSOut;
	std::string ADSWalk;
	std::vector<std::string> ADSFire;
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