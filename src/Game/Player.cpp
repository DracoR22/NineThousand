#include "Player.h"

Player::Player(glm::vec3 position, float height, float mass)
	: m_velocity(0.0f), m_speed(9.5f), camera(position), m_height(height) {
    float eyeHeight = position.y + (height * 0.8f);

	camera = Camera(glm::vec3(position.x, eyeHeight, position.z));

  /*  Physics::CreateCharacterActor(position, height, mass);*/
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

    if (Keyboard::KeyJustPressed(GLFW_KEY_SPACE)) {
        /*Physics::CharacterActorJump();*/
        Physics::UpdatePlayerControllerVerticalVelocity();
    }

    // Normalize move direction to avoid faster diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

     m_isMoving = glm::length(moveDirection) > 0.0f;

     Physics::MovePlayerController(moveDirection * m_speed * static_cast<float>(deltaTime), deltaTime);

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

     camera.setPosition(targetPosition); 
}

glm::vec3 Player::getPosition() {
	return camera.cameraPos;
}

bool Player::IsMoving() {
    return m_isMoving;
}

bool Player::CanFireWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* weaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");

    if (
        m_weaponAction == WeaponAction::IDLE ||
        m_weaponAction == WeaponAction::WALK ||
        m_weaponAction == WeaponAction::FIRE && weaponAnimator->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.fire)
        ) {
        return true;
    }
    else {
        return false;
    }
}

void Player::EquipWeapon(std::string weaponName) {
    WeaponInfo* weapon = WeaponManager::GetWeaponInfoByName(weaponName);
    m_equippedWeapon = weapon;
    std::cout << "Equipped weapon: " << weapon->name << std::endl;
}

WeaponInfo* Player::GetEquipedWeaponInfo() {
    return m_equippedWeapon;
}

WeaponAction Player::GetWeaponAction() {
    return m_weaponAction;
}

bool Player::PressingFire() {
    if (Mouse::button(GLFW_MOUSE_BUTTON_LEFT)) {
        return true;
    }
    else {
        return false;
    }
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

bool Player::PressedReload() {
    if (Keyboard::KeyJustPressed(GLFW_KEY_R)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::ReleasedADS() {
    if (Mouse::buttonWentUp(GLFW_MOUSE_BUTTON_RIGHT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::IsInADS() {
    return (m_weaponAction == WeaponAction::ADS_WALK ||
        m_weaponAction == WeaponAction::ADS_OUT ||
        m_weaponAction == WeaponAction::ADS_IN ||
        m_weaponAction == WeaponAction::ADS_FIRE ||
        m_weaponAction == WeaponAction::ADS_IDLE);
}

void Player::ReloadWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    float animationSpeed = 1.0f;

    currentWeaponAnimator->PlayAnimation(AssetManager::GetAnimationByName(weaponInfo->animations.reload), animationSpeed);
    AudioManager::PlayAudio(weaponInfo->audioFiles.reload, 1.0f, 1.0f);
    m_weaponAction = WeaponAction::RELOAD;  
}

void Player::FireWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
   
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    Animation* weaponReloadAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.reload);
    Animation* weaponFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.fire[0]);
    Animation* weaponADSFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSFire[0]);

    if (PressingFire() && CanFireWeapon()) {
        if (PressingADS()) {
            currentWeaponAnimator->PlayAnimation(weaponADSFireAnimation);
            m_weaponAction = WeaponAction::ADS_FIRE;
        }
        else {
            currentWeaponAnimator->PlayAnimation(weaponFireAnimation);
            m_weaponAction = WeaponAction::FIRE;
        }
        
        int rand = std::rand() % weaponInfo->audioFiles.fire.size();
        AudioManager::PlayAudio(weaponInfo->audioFiles.fire[rand], 1.0f, 1.0f);

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

void Player::EnterADS() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Animation* weaponADSInAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIn);
    Animation* weaponADSIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIdle);

    if (PressedADS()) {
        currentWeaponAnimator->PlayAnimation(weaponADSInAnimation, 1.5f);
        SetWeaponAction(WeaponAction::ADS_IN);
    }

    if (!IsMoving() && PressingADS() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSInAnimation) {
        if (currentWeaponAnimator->IsAnimationFinished()) {
            currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
            SetWeaponAction(WeaponAction::ADS_IDLE);
        }
    }
}

void Player::LeaveADS() {
    if (ReleasedADS()) {
        WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
        Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
        Animation* weaponADSOutAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSOut);

        currentWeaponAnimator->PlayAnimation(weaponADSOutAnimation);
        SetWeaponAction(WeaponAction::ADS_OUT);
    }
}

void Player::UpdateWeaponLogic() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");

    Animation* weaponIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.idle);
    Animation* weaponADSIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIdle);
    Animation* weaponADSInAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIn);
    Animation* weaponWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.walk);

    if (PressedReload()) {
        ReloadWeapon();
    }

    // what if player tries to reload while pressing ADS key?
    if (PressingADS() && m_weaponAction == WeaponAction::RELOAD && currentWeaponAnimator->IsAnimationFinished()) {
        currentWeaponAnimator->PlayAnimation(weaponADSInAnimation, 1.5f);
        SetWeaponAction(WeaponAction::ADS_IN);
    }

    // regular walk animation
    if (!PressingADS() && IsMoving() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponWalkAnimation);
        m_weaponAction = WeaponAction::WALK;
    }

    // if its moving and ADS is pressed
    if (IsMoving() && PressingADS() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSInAnimation) {
        Animation* weaponADSWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSWalk);
        if (currentWeaponAnimator->IsAnimationFinished()) {
            currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
            m_weaponAction = WeaponAction::ADS_WALK;
        }
    }

    // if ADS is pressed and then move
    if (PressingADS() && IsMoving() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSIdleAnimation) {
        Animation* weaponADSWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSWalk);
        currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
        m_weaponAction = WeaponAction::ADS_WALK;
    }

    // go back to idle when getting out ADS
    if (!PressingADS() && m_weaponAction == WeaponAction::ADS_OUT) {
        if (currentWeaponAnimator->IsAnimationFinished()) {
            currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
            m_weaponAction = WeaponAction::IDLE;
        }
    }

    // go back to idle after finishing any non-ads action
    if (!PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() != weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
        m_weaponAction = WeaponAction::IDLE;
    }

    // go back to ads-idle after any ads action
    if (PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() != weaponIdleAnimation && m_weaponAction != WeaponAction::RELOAD) {
        currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
        m_weaponAction = WeaponAction::ADS_IDLE;
    }

    // loop idle animation
    if (!PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
        m_weaponAction = WeaponAction::IDLE;
    }
}

void Player::SetWeaponAction(WeaponAction action) {
    m_weaponAction = action;
}
