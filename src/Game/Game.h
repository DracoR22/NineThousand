#pragma once

#include <vector>

#include "./Player.h"
#include "./WeaponManager.h"
#include "./Scene.h"
#include "./Stalker.h"
#include "./Player.h"

#include "../Core/AssetManager.h"

#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

namespace Game {
	enum class GameState { PLAYING, PAUSED, EDITOR };

	void Init();
	void Update(double deltaTime);
	void UpdatePhysics();

	void CreatePlayers();

	Player& GetPLayerByIndex(int index);
	std::vector<Stalker>& GetAllStalkers();

	void UpdateWeaponPositionByName(std::string weaponName, bool flipRotation = false);

	GameState GetGameState();
	void SetGameState(GameState newState);
}