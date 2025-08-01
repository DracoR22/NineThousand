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

struct AmmoInfo {
	std::string caseModelName = "";
	std::string caseMeshName = "";
	std::string caseMaterialName = "";
	float caseSize = 1.0f;
	glm::vec3 caseSpawnOffset = glm::vec3(0.0f);
	glm::vec3 caseSpawnOffsetADS = glm::vec3(0.0f);
};

struct WeaponInfo {
	std::string name = "";
	std::string modelName = "";
	WeaponAnimations animations;
	WeaponAnimationCancelFrames animationCancelFrames;
	WeaponAudio audioFiles;
	glm::vec3 muzzleFlashOffset = glm::vec3(0.0f);
	glm::vec3 ADSMuzzleFlashOffset = glm::vec3(0.0f);
	glm::vec3 muzzleFlashSize = glm::vec3(0.0f);
	WeaponType type;
	int magSize = 0;
	int maxAmmo = 0;	
	bool hasADS = false;
	std::unordered_map<std::string, std::string> meshMaterials;
	AmmoInfo ammoInfo;
};

struct WeaponState {
	int ammoInMag = 0;
	bool waitingForReload = false;
};

namespace WeaponManager {
	void Init();
	WeaponInfo* GetWeaponInfoByName(std::string& name);
	std::vector<WeaponInfo>& GetAllWeaponInfos();
}