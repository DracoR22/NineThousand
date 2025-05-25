#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "../Core/Model.h"
#include "../Physics/Physics.h"

enum class WeaponAction {
	IDLE,
	DRAW,
    WALK,
	RELOAD,
	RELOAD_EMPTY,
	FIRE,
	ADS_IN,
	ADS_OUT,
	ADS_IDLE,
	ADS_FIRE,
	ADS_WALK
};

enum class WeaponType {
	MELEE,
	SEMI,
	AUTO
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

struct WeaponAnimationCancelFrames {
	int fire = 0;
	int reload = 0;
	int reloadEmpty = 0;
	int draw = 0;
};

struct WeaponAudio {
	std::vector<std::string> fire;
	std::string reload;
	std::string reloadEmpty;
	std::string draw;
};

struct WeaponInfo {
	std::string name = "";
	std::string modelName = "";
	WeaponAnimations animations;
	WeaponAnimationCancelFrames animationCancelFrames;
	WeaponAudio audioFiles;
	glm::vec3 muzzleFlashOffset = glm::vec3(0.0f);
	float muzzleFlashSize = 0.0f;
	WeaponType type;
};

namespace WeaponManager {
	void Init();
	WeaponInfo* GetWeaponInfoByName(std::string& name);
}