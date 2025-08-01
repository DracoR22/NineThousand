#include "Player.h"

Player::Player(glm::vec3 position, float height)
	: m_speed(12.5f), m_camera(position), m_height(height) {
    float eyeHeight = position.y + (height * 0.8f);

	m_camera = Camera(glm::vec3(position.x, eyeHeight, position.z));
    m_position = position;

    //Physics::CreateCharacterController();

    uint64_t physicsId = Physics::CreateCharacterController(position, height);
    m_physicsId = physicsId;

    GameObjectCreateInfo gunCreateInfo{
            "Player1Glock", 
            "Glock",
            glm::vec3(0.0f, 5.0f, 1.0f),
            glm::vec3(0.05f),
            glm::vec3(0.0f),
    };

    m_currentWeaponGameObject = GameObject(gunCreateInfo);
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
    const float smoothingFactor = 0.95f; // Lower = smoother, but more latency

    double dx = Mouse::getDX();
    double dy = Mouse::getDY();

    // Smooth mouse input
    smoothedDelta.x += (dx - smoothedDelta.x) * smoothingFactor;
    smoothedDelta.y += (dy - smoothedDelta.y) * smoothingFactor;

    float sensitivity = 0.007f;
    m_camera.updateCameraDirection(smoothedDelta.x * sensitivity, smoothedDelta.y * sensitivity);

    glm::vec3 characterControllerPos = Physics::GetCharacterControllerPosition(m_physicsId);

    m_position = characterControllerPos;
    glm::vec3 currentCamPos = m_camera.cameraPos;
    glm::vec3 targetCamPos = glm::vec3(characterControllerPos.x, characterControllerPos.y + m_height * 0.8f, characterControllerPos.z);

    float camSmoothFactor = 10.0f; // tweak as needed
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
            AudioManager::PlayAudio(sloshFootSteps[randAudio], 0.5f, 1.0f);

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

void Player::EquipWeapon(std::string weaponName) {
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

        m_weaponStates[weapon.name] = state;
    }
}

WeaponInfo* Player::GetEquipedWeaponInfo() {
    return m_equippedWeapon;
}

WeaponState* Player::GetEquipedWeaponState() {
    if (!m_equippedWeapon) return nullptr;

    const std::string& weaponName = m_equippedWeapon->name;

    return &m_weaponStates[weaponName];
}

WeaponAction Player::GetWeaponAction() {
    return m_weaponAction;
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

        glm::vec3 playerPos = m_camera.cameraPos;
        glm::vec3 cameraDir = glm::normalize(m_camera.cameraFront);

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
        currentWeaponAnimator->PlayAnimation(weaponADSInAnimation, 2.5f);
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
    if (ReleasedADS() && m_weaponAction != WeaponAction::RELOAD && m_weaponAction != WeaponAction::RELOAD_EMPTY) {
        WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
        Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");
        Animation* weaponADSOutAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSOut);

        currentWeaponAnimator->PlayAnimation(weaponADSOutAnimation, 1.5f);
        SetWeaponAction(WeaponAction::ADS_OUT);
    }
}

void Player::UpdateWeaponLogic() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();
    WeaponState* weaponState = GetEquipedWeaponState();
    Animator* currentWeaponAnimator = AssetManager::GetAnimatorByName(weaponInfo->name + "Animator");

    Animation* weaponIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.idle);
  /*  Animation* weaponADSIdleAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIdle);
    Animation* weaponADSInAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSIn);*/
    Animation* weaponWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.walk);

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

    if (CanEnterADS()) {
        EnterADS();
    }

    if (IsInADS()) {
        LeaveADS();
    }

    // regular walk animation
    if (!PressingADS() && IsMoving() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponWalkAnimation);
        m_weaponAction = WeaponAction::WALK;
    }

    // if its moving and ADS is pressed
    //if (IsMoving() && PressingADS() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSInAnimation) {
    //    Animation* weaponADSWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSWalk);
    //    if (currentWeaponAnimator->IsAnimationFinished()) {
    //        currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
    //        m_weaponAction = WeaponAction::ADS_WALK;
    //    }
    //}

    // if ADS is pressed and then move
  /*  if (PressingADS() && IsMoving() && currentWeaponAnimator->GetCurrentAnimation() == weaponADSIdleAnimation) {
        Animation* weaponADSWalkAnimation = AssetManager::GetAnimationByName(weaponInfo->animations.ADSWalk);
        currentWeaponAnimator->PlayAnimation(weaponADSWalkAnimation);
        m_weaponAction = WeaponAction::ADS_WALK;
    }*/

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
 /*   if (PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() != weaponIdleAnimation && m_weaponAction != WeaponAction::RELOAD) {
        currentWeaponAnimator->PlayAnimation(weaponADSIdleAnimation);
        m_weaponAction = WeaponAction::ADS_IDLE;
    }*/

    // cut walk animation if player stops moving!
    //if (m_weaponAction == WeaponAction::WALK && !IsMoving()) {
    //    currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
    //    m_weaponAction = WeaponAction::IDLE;
    //}

    // loop idle animation
    if (!PressingADS() && currentWeaponAnimator->IsAnimationFinished() && currentWeaponAnimator->GetCurrentAnimation() == weaponIdleAnimation) {
        currentWeaponAnimator->PlayAnimation(weaponIdleAnimation);
        m_weaponAction = WeaponAction::IDLE;
    }
}

void Player::SetWeaponAction(WeaponAction action) {
    m_weaponAction = action;
}

void Player::SpawnBulletCase() {
    WeaponInfo* weaponInfo = GetEquipedWeaponInfo();

    // remove previous bullet case if it exists
    if (m_bulletCasePhysicsId != -1) {
        Physics::MarkRigidDynamicForRemoval(m_bulletCasePhysicsId);
        m_bulletCasePhysicsId = -1;
    }
    Scene::RemoveGameObjectByName("BulletCase1");

    // initialize bullet case game object
    GameObjectCreateInfo createInfo;
    createInfo.modelName = weaponInfo->ammoInfo.caseModelName;
    createInfo.name = "BulletCase1";

    // build mesh material
    int caseMaterialIndex = AssetManager::GetMaterialIndexByName(weaponInfo->ammoInfo.caseMaterialName);
    int caseMeshIndex = AssetManager::GetMeshIndexByName(weaponInfo->ammoInfo.caseMeshName);
    MeshRenderingInfo meshInfo;
    meshInfo.materialIndex = caseMaterialIndex;
    meshInfo.meshIndex = caseMeshIndex;
    createInfo.meshRenderingInfo.push_back(meshInfo);

    // apply case spawn position based on the weapon position
    glm::vec3 bulletCaseOffset = weaponInfo->ammoInfo.caseSpawnOffset;
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

    // spawn the case
    Scene::AddGameObject(createInfo);

    // create rigid dynamic
    float caseMass = 0.05f;
    float ejectionForce = 7.0f;

    PhysicsTransformData physicsTransformData;
    physicsTransformData.position = createInfo.position;
    physicsTransformData.rotation = Utils::GlmVec3ToGlmQuat(createInfo.rotation);

    // I tweak the initial force direction if the model is rotated or if I rotate it during ADS animation
    glm::vec3 initialForce = glm::vec3(2.0f, 3.0f, -1.5f) * ejectionForce;
    if (IsInADS() && weaponInfo->ammoInfo.caseSpawnOffsetADS.z > 0 || !IsInADS() && weaponInfo->ammoInfo.caseSpawnOffset.z > 0) {
        initialForce = glm::vec3(-2.0f, 3.0f, 1.5f) * ejectionForce;
    }

    initialForce = glm::vec3(gunTransform * glm::vec4(initialForce, 0.0f)); // transform to world space
    glm::vec3 initialTorque = glm::vec3(10.0f, 20.0f, 5.0f);

    uint64_t physicsId = Physics::CreateRigidDynamicBox(
        physicsTransformData,
        createInfo.size * 0.5f,
        caseMass,
        initialForce,
        initialTorque
    );

    m_bulletCasePhysicsId = physicsId;
}
