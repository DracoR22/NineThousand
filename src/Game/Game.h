#pragma once

#include <vector>

#include "Player.h"
#include "../Core/AssetManager.h"

namespace Game {
	inline std::vector<Player> g_players;

	void Update();

	void CreatePlayers();
	Player& GetPLayerByIndex(int index);
}