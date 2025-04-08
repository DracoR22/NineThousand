#include "Player.h"

Player::Player(glm::vec3 position, float height, float mass)
	: m_velocity(0.0f), m_speed(9.5f), camera(position), m_height(height) {
    float eyeHeight = position.y + (height * 0.8f);

	camera = Camera(glm::vec3(position.x, eyeHeight, position.z));

    Physics::CreateCharacterActor(position, height, mass);
    Physics::InitializeCharacterController();
}

void Player::processInput(double deltaTime) {
    float moveSpeed = m_speed;

    // Calculate direction based on keyboard input
    glm::vec3 moveDirection(0.0f, 0.0f, 0.0f);

    if (Keyboard::KeyPressed(GLFW_KEY_W)) {
        moveDirection += camera.cameraFront;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_S)) {
        moveDirection -= camera.cameraFront;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_D)) {
        moveDirection += camera.cameraRight;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_A)) {
        moveDirection -= camera.cameraRight;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        moveDirection -= camera.cameraUp;
    }

    if (Keyboard::KeyJustPressed(GLFW_KEY_SPACE) && m_isOnGround) {
        /*Physics::CharacterActorJump();*/
        Physics::UpdatePlayerControllerVerticalVelocity();
    }

    // Normalize move direction to avoid faster diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

     m_isMoving = glm::length(moveDirection) > 0.0f;

     Physics::MovePlayerController(moveDirection * m_speed * static_cast<float>(deltaTime), deltaTime);

     //Physics::MoveCharacterActor(targetVelocity);

     // Handle mouse input for camera rotation
     static glm::vec2 smoothedDelta = glm::vec2(0.0f);
     const float smoothingFactor = 0.55f; // Lower = smoother, but more latency

     double dx = Mouse::getDX();
     double dy = Mouse::getDY();

     // Smooth mouse input
     smoothedDelta.x += (dx - smoothedDelta.x) * smoothingFactor;
     smoothedDelta.y += (dy - smoothedDelta.y) * smoothingFactor;

     float sensitivity = 0.01f;
     camera.updateCameraDirection(smoothedDelta.x * sensitivity, smoothedDelta.y * sensitivity);

     physx::PxExtendedVec3 playerPos = Physics::GetPlayerControllerPosition();
     glm::vec3 targetPosition(playerPos.x, playerPos.y + (m_height * 0.8f), playerPos.z);
     const float cameraLag = 0.05f;
     camera.setPosition(targetPosition);

     // Smooth camera movement (dampening)
     //glm::vec3 targetPosition = camera.cameraPos + moveDirection * moveSpeed * static_cast<float>(deltaTime);
     //const float cameraLag = 0.1f; // Lower value = more lag
     //camera.cameraPos += (targetPosition - camera.cameraPos) * cameraLag;

    // Update camera position, TODO: Update Player itself instead
  /*  physx::PxTransform actorTransform = Physics::GetCharacterActorPosition();
    glm::vec3 actorPosition(actorTransform.p.x, actorTransform.p.y, actorTransform.p.z);
    float eyeHeightOffset = height * 0.8f;
    glm::vec3 adjustedCameraPosition = actorPosition + glm::vec3(0.0f, eyeHeightOffset, 0.0f);
    camera.setPosition(adjustedCameraPosition);

    if (actorPosition.y <= 0.5f) {
        m_isOnGround = true;
    }*/
    
}

glm::vec3 Player::getPosition() {
	return camera.cameraPos;
}

bool Player::IsMoving() {
    return m_isMoving;
}

void Player::EquipWeapon(std::string weaponName) {
    WeaponInfo* weapon = WeaponManager::GetWeaponInfoByName(weaponName);
    m_equippedWeapon = weapon;
    std::cout << "Equipped weapon: " << weapon->name << std::endl;


}

WeaponInfo* Player::GetEquipedWeaponInfo() {
    return m_equippedWeapon;
}

bool Player::PressingADS() {
    if (Mouse::button(GLFW_MOUSE_BUTTON_RIGHT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::PressedADS() {
    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_RIGHT)) {
        return true;
    }
    else {
        return false;
    }
}

void Player::ReloadWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    float animationSpeed = 1.0f;

    if (Keyboard::KeyJustPressed(GLFW_KEY_R)) {
        currentWeaponAnimator->PlayAnimation(AssetManager::GetAnimationByName(weaponInfo->animations.reload), animationSpeed);
    }
}

void Player::FireWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
   
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    Animation* weaponReloadAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.reload);
    Animation* weaponFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.fire[0]);
    Animation* glockADSFire1Animation = AssetManager::GetAnimationByName("Glock_ADS_Fire1");

    if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT) && currentWeaponAnimator->GetCurrentAnimation() != weaponReloadAnimation) {
        if (PressingADS()) {
            currentWeaponAnimator->PlayAnimation(glockADSFire1Animation);
            m_ADSFireAnimationFinishTime = 0.0f;
        }
        else {
            currentWeaponAnimator->PlayAnimation(weaponFireAnimation);
        }

        glm::vec3 playerPos = camera.cameraPos;
        glm::vec3 cameraDir = glm::normalize(camera.cameraFront);

        physx::PxVec3 origin(playerPos.x, playerPos.y, playerPos.z);
        physx::PxVec3 direction(cameraDir.x, cameraDir.y, cameraDir.z);

        // Raycast parameters
        float maxDistance = 100000.0f;  // Maximum bullet range
        physx::PxRaycastBuffer hitBuffer; // Stores raycast result

        bool hit = Physics::GetScene()->raycast(origin, direction, maxDistance, hitBuffer);

        if (hit) {
            const physx::PxRaycastHit& hitInfo = hitBuffer.block;

            std::cout << "Hit object at: "
                << hitInfo.position.x << ", "
                << hitInfo.position.y << ", "
                << hitInfo.position.z << std::endl;

            physx::PxRigidDynamic* dynamicActor = hitInfo.actor->is<physx::PxRigidDynamic>();

            if (dynamicActor) {
                physx::PxVec3 impulseDirection = direction.getNormalized();
                float impulseStrength = 500.0f;  // bullet force
                dynamicActor->addForce(impulseDirection * impulseStrength, physx::PxForceMode::eIMPULSE);
            }

        }
    }
}
