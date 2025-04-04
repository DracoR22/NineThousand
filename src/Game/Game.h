#pragma once

#include <vector>

#include "Player.h"
#include "WeaponManager.h"

#include "../Core/AssetManager.h"

#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

namespace Game {
	enum class GameState { PLAYING, PAUSED, EDITOR };

	inline std::vector<Player> g_players;

	void Init();
	void Update(double deltaTime);

	void CreatePlayers();
	Player& GetPLayerByIndex(int index);

	void UpdateWeaponPositionByName(std::string weaponName, bool flipRotation = false);

	GameState GetGameState();
	void SetGameState(GameState newState);
}