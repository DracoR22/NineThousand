#include "Game.h"

namespace Game {

	void Update() {
		// Weapons
		Model* glockModel = AssetManager::GetModelByName("Glock");

		glm::vec3 gunPosition = g_players[0].getPosition() +
			(g_players[0].camera.cameraFront * 0.7f) +   // Offset forward
			(g_players[0].camera.cameraUp * -3.85f);    // Offset downward

		// Calculate gun rotation to align with the camera
		glm::mat4 gunRotation = glm::mat4(1.0f);

		// Rotate around camera right (tilt up/down)
		float theta = acos(glm::dot(g_players[0].camera.worldUp, g_players[0].camera.cameraFront) /
			(glm::length(g_players[0].camera.worldUp) * glm::length(g_players[0].camera.cameraFront)));
		gunRotation = glm::rotate(gunRotation, glm::half_pi<float>() - theta, g_players[0].camera.cameraRight);

		// Rotate around camera up (align gun to camera facing direction in horizontal plane)
		glm::vec2 front2D = glm::vec2(g_players[0].camera.cameraFront.x, g_players[0].camera.cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), glm::normalize(front2D)));
		gunRotation = glm::rotate(gunRotation, g_players[0].camera.cameraFront.z < 0 ? theta : -theta, g_players[0].camera.worldUp);

		// Apply local rotation adjustment to align gun's forward direction
		glm::mat4 localRotationFix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Adjust for left-facing gun
		localRotationFix = glm::rotate(localRotationFix, glm::radians(5.0f), glm::vec3(1.9f, 0.0f, 0.0f)); // Slight upward pitch adjustment
		gunRotation = gunRotation * localRotationFix;

		// Apply transformations to the gun
		glockModel->setPosition(gunPosition);
		glockModel->setRotation(gunRotation);
	}

	void CreatePlayers() {
		Player player(glm::vec3(0.0f, 1.8f, 0.0f), 2.3f, 75.0f);

		g_players.push_back(player);
	}

	Player& GetPLayerByIndex(int index) {
		if (index < 0 || index >= g_players.size()) {
			throw std::out_of_range("ERROR::GetPlayerByIndex::Index out of range!");
		}
		return g_players[index];
	}
}