#include "Player.h"

Player::Player(glm::vec3 position, float height, float mass)
	: velocity(0.0f), speed(1.0f), camera(position) {
	camera = Camera(position);
}



glm::vec3 Player::getPosition() {
	return camera.cameraPos;
}

