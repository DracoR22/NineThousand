#pragma once

#include <vector>

#include "Player.h"
#include "WeaponManager.h"

#include "../Core/AssetManager.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

namespace Game {
	inline std::vector<Player> g_players;

	void Init();
	void Update(double deltaTime);

	void CreatePlayers();
	Player& GetPLayerByIndex(int index);

	void UpdateWeaponPositionByName(std::string weaponName);
}