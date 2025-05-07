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
		glock.animations.fire.push_back("Glock_Fire1");
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

		WeaponInfo& p90 = g_weapons.emplace_back();
		p90.name = "P90";
		p90.modelName = "P90";
		p90.animations.idle = "P90_Idle";
		p90.animations.walk = "P90_Walk";
		p90.animations.reload = "P90_Reload";
		p90.animations.reloadEmpty = "P90_ReloadEmpty";
		p90.animations.fire.push_back("P90_Fire0");
		p90.animations.fire.push_back("P90_Fire1");
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

		WeaponInfo& aks74u = g_weapons.emplace_back();
		aks74u.name = "AKS74U";
		aks74u.modelName = "AKS74U";
		aks74u.animations.idle = "AKS74U_Idle";
		aks74u.animations.walk = "AKS74U_Walk";
		aks74u.animations.reload = "AKS74U_Reload";
		aks74u.animations.reloadEmpty = "AKS74U_ReloadEmpty";
		aks74u.animations.fire.push_back("AKS74U_Fire0");
		aks74u.animations.fire.push_back("AKS74U_Fire1");
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