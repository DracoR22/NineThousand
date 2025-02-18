#pragma once

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

class Player {
public:
	Player(glm::vec3 position, float height, float mass);

	void update(float deltaTime);

    void processInput(double deltaTime);

	Camera camera;

	glm::vec3 getPosition();

public:
	bool isOnGround = true;

private:
	glm::vec3 velocity;
	float speed;
	float height;
	float verticalVelocity = 0.0f;
};