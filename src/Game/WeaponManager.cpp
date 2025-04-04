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


		WeaponInfo& p90 = g_weapons.emplace_back();
		p90.name = "P90";
		p90.modelName = "P90";
		p90.animations.idle = "P90_Idle";
		p90.animations.walk = "P90_Walk";
		p90.animations.reload = "P90_Reload";
		p90.animations.reloadEmpty = "P90_ReloadEmpty";
		p90.animations.fire.push_back("P90_Fire0");
		p90.animations.fire.push_back("P90_Fire1");

		WeaponInfo& aks74u = g_weapons.emplace_back();
		aks74u.name = "AKS74U";
		aks74u.modelName = "AKS74U";
		aks74u.animations.idle = "AKS74U_Idle";
		aks74u.animations.walk = "AKS74U_Walk";
		aks74u.animations.reload = "AKS74U_Reload";
		aks74u.animations.reloadEmpty = "AKS74U_ReloadEmpty";
		aks74u.animations.fire.push_back("AKS74U_Fire0");
		aks74u.animations.fire.push_back("AKS74U_Fire1");
	}

	WeaponInfo* GetWeaponInfoByName(std::string name) {
		for (int i = 0; i < g_weapons.size(); i++) {
			if (g_weapons[i].name == name) {
				return &g_weapons[i];
			}
		}

		std::cout << "WeaponManager::GetWeaponInfoByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}
}