#pragma once

#include "../Input/Camera.h"

class Player {
public:
	Player(glm::vec3 position, float height, float mass);
	~Player();

	void update(float deltaTime);

	Camera camera;

	glm::mat4 getViewMatix();
	glm::vec3 getPosition();

private:
	glm::vec3 velocity;
	float speed;
};