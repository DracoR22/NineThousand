#include "Player.h"

Player::Player(glm::vec3 position, float height, float mass)
	: velocity(0.0f), speed(2.0f), camera(position), height(height) {
    float eyeHeight = position.y + (height * 0.8f);

	camera = Camera(glm::vec3(position.x, eyeHeight, position.z));

    Physics::CreateCharacterActor(position, height, mass);
}

void Player::processInput(double deltaTime) {
    float moveSpeed = speed;

    // Calculate direction based on keyboard input
    glm::vec3 moveDirection(0.0f, 0.0f, 0.0f);

    if (Keyboard::key(GLFW_KEY_W)) {
        moveDirection += camera.cameraFront;
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        moveDirection -= camera.cameraFront;
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        moveDirection += camera.cameraRight;
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        moveDirection -= camera.cameraRight;
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
        moveDirection -= camera.cameraUp;
    }
 /*   if (Keyboard::key(GLFW_KEY_W)) {
        camera.updateCameraPos(CameraDirection::FORWARD, deltaTime);
       
    }
    if (Keyboard::key(GLFW_KEY_S)) {
        camera.updateCameraPos(CameraDirection::BACKWARD, deltaTime);
    }
    if (Keyboard::key(GLFW_KEY_D)) {
        camera.updateCameraPos(CameraDirection::RIGHTWARD, deltaTime);
    }
    if (Keyboard::key(GLFW_KEY_A)) {
        camera.updateCameraPos(CameraDirection::LEFTWARD, deltaTime);
    }
    if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
     
    }

    if (Keyboard::keyWentDown(GLFW_KEY_SPACE) && isOnGround) {
        Physics::CharacterActorJump();
    }*/

    // Normalize move direction to avoid faster diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

     glm::vec3 targetVelocity = moveDirection * moveSpeed;

     Physics::MoveCharacterActor(targetVelocity);

     // Handle mouse input for camera rotation
    double dx = Mouse::getDX(), dy = Mouse::getDY();
    float sensitivity = 0.05f;
    if (dx != 0 || dy != 0) {
        camera.updateCameraDirection(dx * sensitivity, dy * sensitivity);
    }

    // Update camera position, TODO: Update Player itself instead
    physx::PxTransform actorTransform = Physics::GetCharacterActorPosition();
    glm::vec3 actorPosition(actorTransform.p.x, actorTransform.p.y, actorTransform.p.z);
    float eyeHeightOffset = height * 0.8f;
    glm::vec3 adjustedCameraPosition = actorPosition + glm::vec3(0.0f, eyeHeightOffset, 0.0f);
    camera.setPosition(adjustedCameraPosition);

    if (actorPosition.y <= 0.5f) {
        isOnGround = true;
    }
    
}

glm::vec3 Player::getPosition() {
	return camera.cameraPos;
}

