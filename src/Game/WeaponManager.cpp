#include "WeaponManager.h"

namespace WeaponManager {
	std::vector<WeaponInfo> g_weapons;

	void Init() {
		// gun assets from https://github.com/livinamuk/Hell2025
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
		glock.animationCancelFrames.reload = 40;
		glock.animationCancelFrames.ADSWalk = 7;
		glock.audioFiles.fire.push_back("Glock_Fire0.wav");
		glock.audioFiles.fire.push_back("Glock_Fire1.wav");
		glock.audioFiles.fire.push_back("Glock_Fire2.wav");
		glock.audioFiles.fire.push_back("Glock_Fire3.wav");
		glock.audioFiles.reload = "Glock_Reload.wav";
		glock.audioFiles.reloadEmpty = "Glock_ReloadEmpty.wav";
		glock.audioFiles.draw = "NextWeapon.wav";
		glock.muzzleFlashOffset = glm::vec3(-1.7f, 2.6f, -3.7f);
		glock.ADSMuzzleFlashOffset = glm::vec3(2.35f, 2.9f, 4.9f);
		glock.muzzleFlashSize = glm::vec3(5.0f, 2.0f, 5.0f);
		glock.type = WeaponType::SEMI;
		glock.magSize = 15;
		glock.maxAmmo = 99;
		glock.hasADS = true;
		glock.meshMaterials["Glock"] = "Glock";
		glock.meshMaterials["ArmsMale"] = "Hands";
		glock.meshMaterials["ArmsFemale"] = "FemaleArms";
		glock.ammoInfo.caseSpawnOffset = glm::vec3(1.0f, 3.7f, -3.7f);
		glock.ammoInfo.caseSpawnOffsetADS = glm::vec3(0.0f, 4.3f, 3.7f);
		glock.ammoInfo.caseModelName = "Bullet_Case_9mm";
		glock.ammoInfo.caseMaterialName = "Bullet_Case_9mm";
		glock.ammoInfo.caseMeshName = "9mm_Shell";
		glock.ammoInfo.caseSize = 5.0f;

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
		katana.hasADS = false;
		katana.meshMaterials["ArmsMale"] = "Hands";
		katana.meshMaterials["ArmsFemale"] = "FemaleArms";
		katana.meshMaterials["Mesh.009"] = "Katana";

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
		aks74u.animationCancelFrames.reload = 40;
		aks74u.animationCancelFrames.ADSWalk = 11;
		aks74u.audioFiles.fire.push_back("AKS74U_Fire0.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire1.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire2.wav");
		aks74u.audioFiles.fire.push_back("AKS74U_Fire3.wav");
		aks74u.audioFiles.reload = "AKS74U_Reload.wav";
		aks74u.audioFiles.reloadEmpty = "AKS74U_ReloadEmpty.wav";
		aks74u.audioFiles.draw = "NextWeapon.wav";
		aks74u.muzzleFlashOffset = glm::vec3(-1.8f, 2.8f, -3.7f);
		aks74u.ADSMuzzleFlashOffset = glm::vec3(-2.35f, 2.9f, -3.7f);
		aks74u.muzzleFlashSize = glm::vec3(5.0f, 2.0f, 5.0f);
		aks74u.type = WeaponType::AUTO;
		aks74u.magSize = 30;
		aks74u.maxAmmo = 300;
		aks74u.hasADS = true;
		aks74u.meshMaterials["ArmsMale"] = "Hands";
		aks74u.meshMaterials["ArmsFemale"] = "FemaleArms";
		aks74u.meshMaterials["AKS74UBarrel"] = "AKS74U_4";
		aks74u.meshMaterials["AKS74UBolt"] = "AKS74U_1";
		aks74u.meshMaterials["AKS74UHandGuard"] = "AKS74U_0";
		aks74u.meshMaterials["AKS74UMag"] = "AKS74U_3";
		aks74u.meshMaterials["AKS74UPistolGrip"] = "AKS74U_2";
		aks74u.meshMaterials["AKS74UReceiver"] = "AKS74U_1";
		aks74u.meshMaterials["AKS74U_ScopeSupport"] = "AKS74U_ScopeSupport";
		aks74u.meshMaterials["AKS74U_ScopeMain"] = "AKS74U_ScopeMain";
		aks74u.meshMaterials["AKS74U_ScopeFrontCap"] = "AKS74U_ScopeVxor";
		aks74u.meshMaterials["AKS74U_ScopeBackCap"] = "AKS74U_ScopeVxor";
		aks74u.ammoInfo.caseSpawnOffset = glm::vec3(1.5f, 3.6f, -3.7f);
		aks74u.ammoInfo.caseSpawnOffsetADS = glm::vec3(0.5f, 3.6f, -3.7f);
		aks74u.ammoInfo.caseModelName = "Bullet_Case_Rifle";
		aks74u.ammoInfo.caseMaterialName = "Bullet_Case_Rifle";
		aks74u.ammoInfo.caseMeshName = "sleeve";
		aks74u.ammoInfo.caseSize = 0.07f;

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
		p90.animationCancelFrames.reload = 50;
		p90.animationCancelFrames.ADSWalk = 7;
		p90.audioFiles.fire.push_back("P90_Fire0.wav");
		p90.audioFiles.fire.push_back("P90_Fire1.wav");
		p90.audioFiles.fire.push_back("P90_Fire2.wav");
		p90.audioFiles.fire.push_back("P90_Fire3.wav");
		p90.audioFiles.reload = "P90_Reload.wav";
		p90.audioFiles.reloadEmpty = "P90_ReloadEmpty.wav";
		p90.audioFiles.draw = "NextWeapon.wav";
		p90.muzzleFlashOffset = glm::vec3(1.35f, 1.4f, 6.7f);
		p90.ADSMuzzleFlashOffset = glm::vec3(2.35f, 2.9f, 6.7f);
		p90.muzzleFlashSize = glm::vec3(5.0f, 2.0f, 5.0f);
		p90.type = WeaponType::AUTO;
		p90.magSize = 50;
		p90.maxAmmo = 550;
		p90.hasADS = true;
		p90.meshMaterials["ArmsMale"] = "Hands";
		p90.meshMaterials["ArmsFemale"] = "FemaleArms";
		p90.meshMaterials["Magazine_low"] = "P90_Mag";
		p90.meshMaterials["Magazine_low2"] = "P90_Mag";
		p90.meshMaterials["LowerReceiver_low"] = "P90_Main";
		p90.meshMaterials["SideRail_low"] = "P90_Rails";
		p90.meshMaterials["TopRailStandard_low"] = "P90_Rails";
		p90.meshMaterials["UpperReceiver_low"] = "P90_FrontEnd";
		p90.meshMaterials["SideRailScrews_low"] = "P90_Rails";
		p90.meshMaterials["Compensator_low"] = "P90_FrontEnd";
		p90.meshMaterials["ChargingHandle_low"] = "P90_FrontEnd";
		p90.meshMaterials["ChargingHandlePlate_low"] = "P90_FrontEnd";
		p90.meshMaterials["Trigger_low"] = "P90_Main";
		p90.meshMaterials["FireSelector_low"] = "P90_Main";
		p90.meshMaterials["MagRelease_low"] = "P90_Main";
		p90.meshMaterials["Velcro_Clip"] = "P90_Sling";
		p90.meshMaterials["Object_0.002"] = "P90_Rails";
		p90.ammoInfo.caseSpawnOffset = glm::vec3(-1.5f, 3.6f, -3.7f);
		p90.ammoInfo.caseModelName = "Bullet_Case_9mm";
		p90.ammoInfo.caseMaterialName = "Bullet_Case_9mm";
		p90.ammoInfo.caseMeshName = "9mm_Shell";
		p90.ammoInfo.caseSize = 5.0f;
		p90.ammoInfo.caseSpawnOffset = glm::vec3(-0.5f, 4.3f, 3.7f);
		p90.ammoInfo.caseSpawnOffsetADS = glm::vec3(-0.5f, 4.3f, 3.7f);
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

	std::vector<WeaponInfo>& GetAllWeaponInfos() {
		return g_weapons;
	}
}