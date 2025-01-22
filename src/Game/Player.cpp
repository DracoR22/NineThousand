#include "Player.h"

Player::Player(glm::vec3 position, float height, float mass)
	: velocity(0.0f), speed(11.0f), camera(position) {
	camera = Camera(position);

	// Convert glm::vec3 to PxVec3
	physx::PxVec3 pxPosition(position.x, position.y, position.z);

	actor = Physics::CreateDynamicCapsule(pxPosition, height / 2.0f, 0.25f, mass);
    actor->setLinearDamping(0.1f);

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

    if (Keyboard::key(GLFW_KEY_SPACE)) {

       /* moveDirection += camera.cameraUp;*/
        float jumpImpulse = 2.0f;
        physx::PxVec3 jumpForce(0.0f, jumpImpulse, 0.0f);
        actor->addForce(jumpForce, physx::PxForceMode::eIMPULSE);
        isOnGround = false;
    }

    // Normalize move direction to avoid faster diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

    physx::PxVec3 currentVelocity = actor->getLinearVelocity();

    glm::vec3 targetVelocity = moveDirection * moveSpeed;
    targetVelocity.y = currentVelocity.y;
    physx::PxVec3 pxTargetVelocity(targetVelocity.x, targetVelocity.y, targetVelocity.z);
    actor->setLinearVelocity(pxTargetVelocity);

    // Handle mouse input for camera rotation
    double dx = Mouse::getDX(), dy = Mouse::getDY();
    float sensitivity = 0.1f;
    if (dx != 0 || dy != 0) {
        camera.updateCameraDirection(dx * sensitivity, dy * sensitivity);
    }

    // Update the camera's position to match the actor's position
    physx::PxTransform actorTransform = actor->getGlobalPose();
    glm::vec3 actorPosition(actorTransform.p.x, actorTransform.p.y, actorTransform.p.z);
    camera.setPosition(actorPosition);

    if (actorPosition.y <= 0.5f) { // Example: check if player is close to the ground
        isOnGround = true;
    }

    //printf("Actor position: (%f, %f, %f)\n", actorPosition.x, actorPosition.y, actorPosition.z);
}

glm::vec3 Player::getPosition() {
	return camera.cameraPos;
}

