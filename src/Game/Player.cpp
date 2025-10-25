#include "Player.h"

#include "../Common/Enums.hpp"

Player::Player(glm::vec3 position, float height)
	: m_speed(12.5f), m_camera(position), m_height(height) {
    float eyeHeight = position.y + (height * 0.8f);

	m_camera = Camera(glm::vec3(position.x, eyeHeight, position.z));
    m_position = position;

    uint64_t objectId = Utils::GenerateUniqueID();

    uint64_t physicsId = Physics::CreateCharacterController(objectId, position, height, ObjectType::PLAYER);
    m_physicsId = physicsId;
    m_objectId = objectId;

    GameObjectCreateInfo gunCreateInfo{
            "Player1Glock", 
            "Glock",
            glm::vec3(0.0f, 5.0f, 1.0f),
            glm::vec3(0.05f),
            glm::vec3(0.0f),
    };

    m_currentWeaponGameObject = GameObject(gunCreateInfo);

    InitWeaponStates();
}

void Player::Update(double deltaTime) {
    std::vector<WaterObject>& waterObjects = Scene::GetWaterPlaneObjects();
  
    glm::vec3 playerPos = m_camera.cameraPos;

    if (waterObjects.empty()) {
        m_isOnWaterSurface = false;
    }
    else {
        glm::vec3 waterPos = waterObjects[0].GetPosition();
        glm::vec3 waterSize = waterObjects[0].GetSize() * 0.5f;

        bool insideXZ = playerPos.x >= (waterPos.x - waterSize.x) && playerPos.x <= (waterPos.x + waterSize.x) &&
            playerPos.z >= (waterPos.z - waterSize.z) && playerPos.z <= (waterPos.z + waterSize.z);

        // TODO: CHECK PLAYER FEET POSITION
        bool aboveWater = playerPos.y >= waterPos.y;

        m_isOnWaterSurface = insideXZ && aboveWater;
    }

    UpdateAudio(deltaTime);
    UpdateMovement(deltaTime);
    UpdateWeaponLogic(deltaTime);
}

void Player::UpdateMovement(double deltaTime) {
    glm::vec3 horizontalFront = glm::normalize(glm::vec3(m_camera.cameraFront.x, 0.0f, m_camera.cameraFront.z));
    glm::vec3 horizontalRight = glm::normalize(glm::vec3(m_camera.cameraRight.x, 0.0f, m_camera.cameraRight.z));

    glm::vec3 moveDirection(0.0f, 0.0f, 0.0f);

    if (Keyboard::KeyPressed(GLFW_KEY_W)) {
        moveDirection += horizontalFront;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_S)) {
        moveDirection -= horizontalFront;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_D)) {
        moveDirection += horizontalRight;
    }
    if (Keyboard::KeyPressed(GLFW_KEY_A)) {
        moveDirection -= horizontalRight;
    }

    float currentSpeed = m_speed;
    if (Keyboard::KeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        currentSpeed *= 2.0f;
    }

    if (Keyboard::KeyJustPressed(GLFW_KEY_SPACE)) {
        Physics::UpdateCharacterControllerVerticalVelocity(m_physicsId);
    }

    // Normalize move direction to avoid faster diagonal movement
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
    }

    m_isMoving = glm::length(moveDirection) > 0.0f;

    Physics::MoveCharacterController(m_physicsId, moveDirection * currentSpeed * static_cast<float>(deltaTime));

    // Handle mouse movement for camera rotation
    static glm::vec2 smoothedDelta = glm::vec2(0.0f);
    const float smoothingFactor = 0.5f; // Lower = smoother, but more latency

    double dx = Mouse::getDX();
    double dy = Mouse::getDY();

    // Smooth mouse input
    smoothedDelta.x += (dx - smoothedDelta.x) * smoothingFactor;
    smoothedDelta.y += (dy - smoothedDelta.y) * smoothingFactor;

    float sensitivity = 0.01f;
    m_camera.updateCameraDirection(smoothedDelta.x * sensitivity, smoothedDelta.y * sensitivity);

    glm::vec3 characterControllerPos = Physics::GetCharacterControllerPosition(m_physicsId);

    m_position = characterControllerPos;
    glm::vec3 currentCamPos = m_camera.cameraPos;
    glm::vec3 targetCamPos = glm::vec3(characterControllerPos.x, characterControllerPos.y + m_height * 0.8f, characterControllerPos.z);

    float camSmoothFactor = 10.0f;
    m_camera.setPosition(glm::mix(currentCamPos, targetCamPos, camSmoothFactor * static_cast<float>(deltaTime)));
    //m_camera.setPosition(targetCamPos);
}

void Player::UpdateAudio(double deltaTime) {
    // steps
    const std::vector<const char*> sloshFootSteps = {
        "slosh1.wav",
        "slosh2.wav",
        "slosh3.wav",
         "slosh4.wav"
    };

    static float m_footstepTimer = 0.0f;
    static float m_footstepInterval = 0.4f;

    if (m_isMoving && m_isOnWaterSurface) {
        m_footstepTimer -= static_cast<float>(deltaTime);

        if (m_footstepTimer <= 0.0f) {
            int randAudio = std::rand() % sloshFootSteps.size();
            AudioManager::PlayAudio(sloshFootSteps[randAudio], 0.7f, 1.0f);

            m_footstepTimer = m_footstepInterval;

            if (IsSprinting()) {
                m_footstepTimer *= 0.85f; 
            }
        }
    }
    else {
        m_footstepTimer = 0.0f;
    }

    // bullet cases
    if (m_bulletCaseSoundCooldown > 0.0f) {
        m_bulletCaseSoundCooldown -= Window::GetDeltaTime();
    }

    WeaponState* weaponState = GetEquipedWeaponState();

    if (ReleasedFire() && weaponState->ammoInMag > 1 && m_bulletCaseSoundCooldown <= 0.0f) {
        AudioManager::PlayAudio("BulletCasingBounce.wav", 1.0f, 1.0f);
        float bulletCaseSoundCooldownDuration = 2.0f;
        m_bulletCaseSoundCooldown = bulletCaseSoundCooldownDuration;
    }

}

glm::vec3 Player::getPosition() {
	return m_camera.cameraPos;
}

bool Player::IsMoving() {
    return m_isMoving;
}

bool Player::IsSprinting() {
    if (IsMoving() && PressingSprintKey()) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::CanReloadWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    WeaponState* weaponState = GetEquipedWeaponState();

    if (!weaponState || !weaponInfo) {
        return false;
    }

    if (m_weaponAction == WeaponAction::RELOAD || m_weaponAction == WeaponAction::RELOAD_EMPTY) {
        return false;
    }

    if (weaponState->ammoInMag >= weaponInfo->magSize) {
        return false;
    }

    return true;
}

bool Player::CanAutoReloadWeapon() {
    WeaponState* weaponState = GetEquipedWeaponState();

    if (m_equippedWeapon->type == WeaponType::MELEE) {
        return false;
    }

    // TODO: check for ammo reserves
    return (weaponState->ammoInMag <= 0 &&
        m_weaponAction != WeaponAction::RELOAD &&
        m_weaponAction != WeaponAction::RELOAD_EMPTY);
}

bool Player::CanEnterADS() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();

    if (m_weaponAction == WeaponAction::RELOAD || m_weaponAction == WeaponAction::RELOAD_EMPTY) {
        return false;
    }

    if (weaponInfo->type == WeaponType::MELEE || !weaponInfo->hasADS) {
        return false;
    }
    else {
        return true;
    }
}

bool Player::CanFireWeapon() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* weaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");

    if (GetEquipedWeaponState()->ammoInMag <= 0) {
        return false;
    }

    if (weaponInfo->type == WeaponType::MELEE) {
        return false;
    }

    if (
        m_weaponAction == WeaponAction::ADS_IDLE ||
        m_weaponAction == WeaponAction::ADS_WALK ||
        m_weaponAction == WeaponAction::ADS_FIRE && weaponAnimator->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.fire) ||
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

void Player::EquipWeapon(const std::string weaponName) {
    WeaponInfo* weapon = WeaponManager::GetWeaponInfoByName(weaponName);
    m_equippedWeapon = weapon;
    std::cout << "Equipped weapon: " << weapon->name << std::endl;
}

void Player::InitWeaponStates() {
    // TODO: MANAGE PLAYER INVENTORY INSTEAD
    std::vector<WeaponInfo>& allWeapons = WeaponManager::GetAllWeaponInfos();

    for (WeaponInfo& weapon : allWeapons) {
        WeaponState state;
        state.ammoInMag = weapon.magSize;
        state.ammoMode = AmmoMode::DEFAULT;

        m_weaponStates[weapon.name] = state;
    }
}

void Player::GiveWeapon(const std::string& weaponName) {
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(weaponName);
    WeaponState* weaponState = GetWeaponStateByName(weaponName);

    if (weaponInfo && weaponState) {
        weaponState->has = true;
        weaponState->ammoInMag = weaponInfo->magSize;
    }
}

void Player::NextWeapon() {
    std::vector<WeaponInfo>& allWeapons = WeaponManager::GetAllWeaponInfos();
    if (allWeapons.empty()) return;

    size_t startIndex = m_currentWeaponIndex;
    do {
        m_currentWeaponIndex = (m_currentWeaponIndex + 1) % allWeapons.size();
        const std::string& weaponName = allWeapons[m_currentWeaponIndex].name;

        if (m_weaponStates[weaponName].has) {
            SwitchWeapon(weaponName, false);
            return;
        }
    } while (m_currentWeaponIndex != startIndex);
}

bool Player::SwitchWeapon(const std::string& weaponName, bool syncIndex) {
    WeaponState* weaponState = GetWeaponStateByName(weaponName);
    WeaponInfo* weaponInfo = WeaponManager::GetWeaponInfoByName(weaponName);

    if (!weaponInfo || !weaponState) {
        std::cout << "Player::SwitchWeapon error: weapon not found -> " << weaponName << std::endl;
        return false;
    }

    if (!weaponState->has) {
        //std::cout << "Player::SwitchWeapon error: player does not own -> " << weaponName << std::endl;
        return false;
    }

    if (m_equippedWeapon == weaponInfo) {
        return false;
    }

    m_equippedWeapon = weaponInfo;

    if (syncIndex) {
        std::vector<WeaponInfo>& allWeapons = WeaponManager::GetAllWeaponInfos();
        for (size_t i = 0; i < allWeapons.size(); ++i) {
            if (allWeapons[i].name == weaponName) {
                m_currentWeaponIndex = i;
                break;
            }
        }
    }

    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Animation* weaponDrawAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.draw);

    currentWeaponAnimator->PlayAnimation(weaponDrawAnimation);
    std::cout << "Equipped weapon: " << weaponInfo->name << std::endl;
    return true;
}

WeaponInfo* Player::GetEquipedWeaponInfo() {
    return m_equippedWeapon;
}

WeaponState* Player::GetEquipedWeaponState() {
    if (!m_equippedWeapon) return nullptr;

    const std::string& weaponName = m_equippedWeapon->name;

    return &m_weaponStates[weaponName];
}

WeaponState* Player::GetWeaponStateByName(const std::string& name) {
    auto it = m_weaponStates.find(name);

    if (it != m_weaponStates.end()) {
        return &it->second;
    }

    return nullptr;
}

WeaponAction Player::GetWeaponAction() {
    return m_weaponAction;
}


std::string Player::GetStringWeaponAction() {
    std::string action = "";

    switch (m_weaponAction) {
    case WeaponAction::ADS_FIRE:
        action = "ADS_FIRE";
        break;
    case WeaponAction::ADS_WALK:
        action = "ADS_WALK";
        break;
    case WeaponAction::ADS_IN:
        action = "ADS_IN";
        break;
    case WeaponAction::ADS_OUT:
        action = "ADS_OUT";
        break;
    case WeaponAction::ADS_IDLE:
        action = "ADS_IDLE";
        break;
    case WeaponAction::IDLE:
        action = "IDLE";
        break;
    case WeaponAction::DRAW:
        action = "DRAW";
        break;
    case WeaponAction::WALK:
        action = "WALK";
        break;
    case WeaponAction::RELOAD:
        action = "RELOAD";
        break;
    case WeaponAction::RELOAD_EMPTY:
        action = "RELOAD_EMPTY";
        break;
    case WeaponAction::FIRE:
        action = "FIRE";
        break;
;    default:
        break;
    }

    return action;
}

bool Player::PressingSprintKey() {
    if (Keyboard::KeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::PressingFire() {
    if (Mouse::ButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::PressedFire() {
    if (Mouse::ButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::PressingADS() {
    if (Mouse::ButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::PressedADS() {
    if (Mouse::ButtonJustPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
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

bool Player::PressedDraw() {
    if (Keyboard::KeyJustPressed(GLFW_KEY_1)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::ReleasedADS() {
    if (Mouse::ButtonJustReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
        return true;
    }
    else {
        return false;
    }
}

bool Player::ReleasedFire() {
    if (Mouse::ButtonJustReleased(GLFW_MOUSE_BUTTON_LEFT)) {
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
    WeaponState* weaponState = GetEquipedWeaponState();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Animation* reloadAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.reload);
    Animation* reloadEmptyAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.reloadEmpty);
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    float animationSpeed = 1.0f;

    if (CanReloadWeapon()) {
        if (weaponState->ammoInMag <= 0) {
            m_weaponAction = WeaponAction::RELOAD_EMPTY;
            currentWeaponAnimator->PlayAnimation(reloadEmptyAnimation, animationSpeed);
            AudioManager::PlayAudio(weaponInfo->audioFiles.reloadEmpty, 1.0f, 1.0f);
        }
        else {
            m_weaponAction = WeaponAction::RELOAD;
            currentWeaponAnimator->PlayAnimation(reloadAnimation, animationSpeed);
            AudioManager::PlayAudio(weaponInfo->audioFiles.reload, 1.0f, 1.0f);
        }

        weaponState->waitingForReload = true;
    }
}

    void Player::FireWeapon() {
        WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
        Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
   
        Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

            if (PressingADS()) {
                Animation* weaponADSFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSFire[0]);
                currentWeaponAnimator->PlayAnimation(weaponADSFireAnimation);
                m_weaponAction = WeaponAction::ADS_FIRE;
            }
            else {
                Animation* weaponFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.fire[0]);
                currentWeaponAnimator->PlayAnimation(weaponFireAnimation);
                m_weaponAction = WeaponAction::FIRE;
            }

        int randAudio = std::rand() % weaponInfo->audioFiles.fire.size();
        AudioManager::PlayAudio(weaponInfo->audioFiles.fire[randAudio], 1.0f, 1.0f);

        m_firedThisFrame = true;

        // spawn muzzle flash
        WeaponState* state = GetEquipedWeaponState();
        if (state && state->ammoInMag > 0) {
            state->ammoInMag -= 1;
            m_muzzleFlashTimer = 5;
        }

        SpawnBulletCase();
    }

void Player::MeleeHit() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
   
    Model* weaponModel = AssetManager::GetModelByName(weaponInfo->name);

    static int currentFireAnimIndex = 0;
    if (currentFireAnimIndex >= weaponInfo->animations.fire.size()) {
        currentFireAnimIndex = 0;
    }

     Animation* weaponFireAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.fire[currentFireAnimIndex]);

     currentWeaponAnimator->PlayAnimation(weaponFireAnimation);
     m_weaponAction = WeaponAction::FIRE;

     int randAudio = std::rand() % weaponInfo->audioFiles.fire.size();
     AudioManager::PlayAudio(weaponInfo->audioFiles.fire[randAudio], 1.0f, 1.0f);

     currentFireAnimIndex++;
}

void Player::EnterADS() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
    Animation* weaponADSInAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIn);
    Animation* weaponADSIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIdle);

    if (PressedADS()) {
        m_camera.SetCameraZoom(35.0f);
        currentWeaponAnimator->PlayAnimation(weaponADSInAnimation, 2.5f);
        SetWeaponAction(WeaponAction::ADS_IN);
    }

   /* if (!IsMoving() && PressingADS() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSInAnimation) {
        if (currentWeaponAnimator->IsAnimationFinished()) {
            currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
            SetWeaponAction(WeaponAction::ADS_IDLE);
        }
    }*/
}

void Player::LeaveADS() {
    if (ReleasedADS() && m_weaponAction != WeaponAction::RELOAD && m_weaponAction != WeaponAction::RELOAD_EMPTY) {
        WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
        Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
        Animation* weaponADSOutAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSOut);

        currentWeaponAnimator->PlayAnimation(weaponADSOutAnimation, 1.5f);
        SetWeaponAction(WeaponAction::ADS_OUT);
    }
}

void Player::UpdateWeaponLogic(double deltaTime) {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    WeaponState* weaponState = GetEquipedWeaponState();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");

    Animation* weaponIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.idle);
    Animation* weaponWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.walk);
    Animation* weaponDrawAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.draw);

    if (PressingFire() && CanFireWeapon()) {
        FireWeapon();
    }

    if (PressedFire() && m_equippedWeapon->type == WeaponType::MELEE) {
        MeleeHit();
    }

    if (PressedReload() || CanAutoReloadWeapon()) {
        ReloadWeapon();
    }

    // only give the ammo after animation is completed
    if (weaponState->waitingForReload && currentWeaponAnimator->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.reload)) {
        weaponState->ammoInMag = weaponInfo->magSize;
        weaponState->waitingForReload = false;
    }

    // draw weapon animation
    if (PressedDraw()) {
        NextWeapon();
        m_weaponAction = WeaponAction::DRAW;
        std::cout << "PLAYING: " << weaponInfo->animations.draw << std::endl;
       /* currentWeaponAnimator->PlayAnimation(weaponDrawAnimation);*/
    }

    // change ammo mode
    if (Keyboard::KeyJustPressed(GLFW_KEY_5)) {
        if (GetEquipedWeaponState()->ammoMode == AmmoMode::DEFAULT) {
            SetAmmoMode(AmmoMode::PLASMA);
        }
        else {
            SetAmmoMode(AmmoMode::DEFAULT);
        }
    }

    if (CanEnterADS()) {
        EnterADS();
    }

    if (IsInADS()) {
        LeaveADS();
    }

    if (!PressingADS() || !IsInADS()) {
        // TODO: use player's fov
        m_camera.SetCameraZoom(45.0f);
    }

    // regular walk animation
    if (!PressingADS() && IsMoving() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponWalkAnimation);
        m_weaponAction = WeaponAction::WALK;
    }
 
    // if player is in ADS state
    if (weaponInfo->hasADS && weaponInfo->name != "P90") {
        Animation* weaponADSIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIdle);
        Animation* weaponADSInAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIn);
        Animation* weaponADSWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSWalk);

        // if ADS is pressed and then move
        if (PressingADS() && IsMoving() && m_weaponAction == WeaponAction::ADS_IDLE || PressingADS() && IsMoving() && (currentWeaponAnimator->IsAnimationFinished() && m_weaponAction == WeaponAction::ADS_WALK)) {
               currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
               m_weaponAction = WeaponAction::ADS_WALK;
        }

        // if its moving and ADS is pressed
        if (IsMoving() && PressingADS() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSInAnimation) {
            if (currentWeaponAnimator->AnimationIsPastFrameNumber(weaponInfo->animationCancelFrames.ADSWalk)) {
                currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
                m_weaponAction = WeaponAction::ADS_WALK;
            }
        }

        // transition from ads fire to ads walk
        if (PressingADS() && IsMoving() && (m_weaponAction == WeaponAction::ADS_FIRE) && currentWeaponAnimator->AnimationIsPastFrameNumber(5)) {
            currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
            m_weaponAction = WeaponAction::ADS_WALK;
        }

      /*  if (PressingADS() && !IsMoving() && m_weaponAction == WeaponAction::ADS_WALK && currentWeaponAnimator->AnimationIsPastFrameNumber(3)) {
            currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
            m_weaponAction = WeaponAction::ADS_IDLE;
        }*/

        // go back to ads-idle after any ads action
        if (PressingADS() && !IsMoving() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() != weaponIdleAnimation && m_weaponAction != WeaponAction::ADS_WALK) {
            //currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
            m_weaponAction = WeaponAction::ADS_IDLE;
        }
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

    // loop idle animation
    if (!PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
        m_weaponAction = WeaponAction::IDLE;
    }

    currentWeaponAnimator->UpdateAnimation(deltaTime);
}

void Player::SetWeaponAction(WeaponAction action) {
    m_weaponAction = action;
}

void Player::SetAmmoMode(AmmoMode mode) {
    WeaponState* weaponState = GetEquipedWeaponState();
    weaponState->ammoMode = mode;
}

void Player::SpawnBulletCase() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    WeaponState* weaponState = GetEquipedWeaponState();

    for (int i = 0; i < Scene::GetBulletCaseObjects().size(); i++) {
        Scene::RemoveBulletCaseObjectByIndex(i);

    }

    BulletCaseCreateInfo createInfo;
    createInfo.modelName = weaponInfo->ammoInfo.caseModelName;
    createInfo.materialIndex = AssetManager::GetMaterialIndexByName(weaponInfo->ammoInfo.caseMaterialName);

    // apply case spawn position based on the weapon position
    glm::vec3 bulletCaseOffset =  weaponInfo->ammoInfo.caseSpawnOffset; //glm::vec3(1.0f, 2.7f, -3.7f);
    if (IsInADS()) {
        bulletCaseOffset = weaponInfo->ammoInfo.caseSpawnOffsetADS;
    }
    glm::mat4 gunTransform = glm::translate(glm::mat4(1.0f), m_currentWeaponGameObject.GetPosition()) * m_currentWeaponGameObject.GetRotationMatrix();
    glm::vec4 worldBarrelPos = gunTransform * glm::vec4(bulletCaseOffset, 1.0f);
    createInfo.position = worldBarrelPos;
    createInfo.size = glm::vec3(weaponInfo->ammoInfo.caseSize);

    // apply case spawn rotation based on the weapon rotation
    glm::quat gunWorldRotation = glm::quat_cast(m_currentWeaponGameObject.GetRotationMatrix());
    glm::quat localCaseRotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)));

    if (weaponInfo->name == "AKS74U") {
        createInfo.rotation = glm::degrees(glm::eulerAngles(gunWorldRotation * localCaseRotation));
    }
    else {
        createInfo.rotation = glm::degrees(glm::eulerAngles(gunWorldRotation));
    }

    // create rigid dynamic
    float caseMass = 0.5f;
    float ejectionForce = 6.0f;

    PhysicsTransformData physicsTransformData;
    physicsTransformData.position = createInfo.position;
    physicsTransformData.rotation = Utils::GlmVec3ToGlmQuat(createInfo.rotation);

    // I tweak the initial force direction if the model is rotated or if I rotate it during ADS animation
    glm::vec3 initialForce = glm::vec3(2.0f, 3.0f, -1.5f) * ejectionForce;
    if (IsInADS() && weaponInfo->ammoInfo.caseSpawnOffsetADS.z > 0 || !IsInADS() && weaponInfo->ammoInfo.caseSpawnOffset.z > 0) {
        initialForce = glm::vec3(-2.0f, 3.0f, 1.5f) * ejectionForce;
    }

    initialForce = glm::vec3(gunTransform * glm::vec4(initialForce, 0.0f)); // transform to world space

    glm::vec3 localTorque = glm::vec3(
        20.0f + Utils::RandomFloat(-20.0f, 20.0f),
        Utils::RandomFloat(-3.0f, 3.0f),
        20.0f + Utils::RandomFloat(0.0f, 20.0f)
    );
    glm::vec3 worldTorque = glm::vec3(gunTransform * glm::vec4(localTorque, 0.0f));

    uint64_t physicsId = Physics::CreateRigidDynamicBox(
        physicsTransformData,
        glm::vec3(0.07f) * 0.5f,
        caseMass,
        initialForce,
        worldTorque
    );

    createInfo.physicsId = physicsId;
    createInfo.isEmissive = weaponState->ammoMode == AmmoMode::DEFAULT ? false : true;

    // spawn the case
    Scene::AddBulletCaseObject(createInfo);

}
