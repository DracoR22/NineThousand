#include "WeaponManager.h"

namespace WeaponManager {
	std::vector<WeaponInfo> g_weapons;

	void Init() {
		g_weapons.clear();

		WeaponInfo& glock = g_weapons.emplace_back();
		glock.name = "Glock";
		glock.modelName = "Glock";
		glock.animations.idle = "Glock_Idle";
		glock.animations.walk = "Glock_Walk";
		glock.animations.reload = "Glock_Reload";
		glock.animations.reloadEmpty = "Glock_ReloadEmpty";
		glock.animations.fire.push_back("Glock_Fire0");
		glock.animations.ADSIdle = "Glock_ADS_Idle";
		glock.animations.ADSIn = "Glock_ADS_In";
		glock.animations.ADSOut = "Glock_ADS_Out";
		glock.animations.ADSWalk = "Glock_ADS_Walk";
		glock.animations.ADSFire.push_back("Glock_ADS_Fire");
		glock.animationCancelFrames.fire = 4;
		glock.audioFiles.fire.push_back("Glock_Fire0.wav");
		glock.audioFiles.fire.push_back("Glock_Fire1.wav");
		glock.audioFiles.fire.push_back("Glock_Fire2.wav");
		glock.audioFiles.fire.push_back("Glock_Fire3.wav");
		glock.audioFiles.reload = "Glock_Reload.wav";
		glock.audioFiles.reloadEmpty = "Glock_ReloadEmpty.wav";
		glock.audioFiles.draw = "NextWeapon.wav";
		glock.muzzleFlashOffset = glm::vec3(0.0f, 0.0f, -15.0f);
		glock.muzzleFlashSize = 4.0f;
		glock.type = WeaponType::SEMI;

		WeaponInfo& katana = g_weapons.emplace_back();
		katana.name = "Katana";
		katana.modelName = "Katana";
		katana.animations.idle = "Knife_Idle";
		katana.animations.walk = "Knife_Walk";
		katana.animations.fire.push_back("Knife_Swing0");
		katana.animations.fire.push_back("Knife_Swing1");
		katana.animations.fire.push_back("Knife_Swing2");
		katana.animations.draw = "Knife_Draw";
		katana.audioFiles.fire.push_back("Katana_Hit1.wav");
		katana.audioFiles.reloadEmpty = "Glock_ReloadEmpty.wav";
		katana.audioFiles.draw = "Katana_Draw.wav";
		katana.animationCancelFrames.fire = 13;
		katana.type = WeaponType::MELEE;

		WeaponInfo& p90 = g_weapons.emplace_back();
		p90.name = "P90";
		p90.modelName = "P90";
		p90.animations.idle = "P90_Idle";
		p90.animations.walk = "P90_Walk";
		p90.animations.reload = "P90_Reload";
		p90.animations.reloadEmpty = "P90_ReloadEmpty";
		p90.animations.fire.push_back("P90_Fire0");
		p90.animations.ADSIdle = "P90_ADS_Idle";
		p90.animations.ADSIn = "P90_ADS_In";
		p90.animations.ADSOut = "P90_ADS_Out";
		p90.animations.ADSWalk = "P90_ADS_Walk";
		p90.animations.ADSFire.push_back("P90_ADS_Fire");
		p90.animationCancelFrames.fire = 2;
		p90.audioFiles.fire.push_back("P90_Fire0.wav");
		p90.audioFiles.fire.push_back("P90_Fire1.wav");
		p90.audioFiles.fire.push_back("P90_Fire2.wav");
		p90.audioFiles.fire.push_back("P90_Fire3.wav");
		p90.audioFiles.reload = "P90_Reload.wav";
		p90.audioFiles.reloadEmpty = "P90_ReloadEmpty.wav";
		p90.audioFiles.draw = "NextWeapon.wav";
		p90.muzzleFlashOffset = glm::vec3(0.0f, 0.0f, 15.0f);
		p90.muzzleFlashSize = 4.0f;
		p90.type = WeaponType::AUTO;

		WeaponInfo& aks74u = g_weapons.emplace_back();
		aks74u.name = "AKS74U";
		aks74u.modelName = "AKS74U";
		aks74u.animations.idle = "AKS74U_Idle";
		aks74u.animations.walk = "AKS74U_Walk";
		aks74u.animations.reload = "AKS74U_Reload";
		aks74u.animations.reloadEmpty = "AKS74U_ReloadEmpty";
		aks74u.animations.fire.push_back("AKS74U_Fire0");
		aks74u.animations.ADSIdle = "AKS74U_ADS_Idle";
		aks74u.animations.ADSIn = "AKS74U_ADS_In";
		aks74u.animations.ADSOut = "AKS74U_ADS_Out";
		aks74u.animations.ADSWalk = "AKS74U_ADS_Walk";
		aks74u.animations.ADSFire.push_back("AKS74U_ADS_Fire");
		aks74u.animationCancelFrames.fire = 2;
		aks74u.audioFiles.fire.push_back("AKS74U_Fire0.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire1.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire2.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire3.wav");
		aks74u.audioFiles.reload = "AKS74U_Reload.wav";
		aks74u.audioFiles.reloadEmpty = "AKS74U_ReloadEmpty.wav";
		aks74u.audioFiles.draw = "NextWeapon.wav";
		aks74u.muzzleFlashOffset = glm::vec3(0.0f, 0.0f, -20.0f);
		aks74u.muzzleFlashSize = 4.0f;
		aks74u.type = WeaponType::AUTO;
	}

	WeaponInfo* GetWeaponInfoByName(std::string& name) {
		for (int i = 0; i < g_weapons.size(); i++) {
			if (g_weapons[i].name == name) {
				return &g_weapons[i];
			}
		}

		std::cout << "WeaponManager::GetWeaponInfoByName() failed because '" << name << "' does not exist\n";
		return nullptr;
	}
}