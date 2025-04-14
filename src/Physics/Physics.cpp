#include "Physics.h"

namespace Physics {
    using namespace physx;
    PxDefaultAllocator _gAllocator;
    PxDefaultErrorCallback _gErrorCallback;
    PxFoundation* _gFoundation = nullptr;
    PxPhysics* _gPhysics = nullptr;
    PxDefaultCpuDispatcher* _gDispatcher = nullptr;
    PxScene* _gScene = nullptr;
    PxMaterial* _gMaterial = nullptr;
    PxMaterial* _gCharacterMaterial = nullptr;
    PxController* _gController = nullptr;


    PxRigidDynamic* _cubeActor = nullptr;
    PxRigidDynamic* _playerActor = nullptr;

    float g_ControllerVerticalVelocity = 0.0f;

    void InitPhysx() {
        _gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _gAllocator, _gErrorCallback);
        _gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_gFoundation, PxTolerancesScale(), true);

        // Create a CPU dispatcher for multithreading
        _gDispatcher = PxDefaultCpuDispatcherCreate(2);

        PxSceneDesc sceneDesc(_gPhysics->getTolerancesScale()); 
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // -9.81 earths gravity
        sceneDesc.cpuDispatcher = _gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        _gScene = _gPhysics->createScene(sceneDesc);

        _gMaterial = _gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
        _gCharacterMaterial = _gPhysics->createMaterial(0.5f, 0.5f, 0.0f);
    }


    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass) {
        PxTransform transform(position);
        PxRigidDynamic* cubeActor = _gPhysics->createRigidDynamic(transform);

        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        cubeActor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*cubeActor, mass);
        _gScene->addActor(*cubeActor);

        _cubeActor = cubeActor;

        return cubeActor;
    }

    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents) {
        PxTransform transform(position);

        PxRigidStatic* staticActor = _gPhysics->createRigidStatic(transform);

        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        staticActor->attachShape(*shape);
        shape->release();

        _gScene->addActor(*staticActor);

        return staticActor;
    }

    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass) {
        PxTransform transform(position);

        PxRigidDynamic* capsuleActor = _gPhysics->createRigidDynamic(transform);

        PxShape* shape = _gPhysics->createShape(PxCapsuleGeometry(radius, halfHeight), *_gMaterial);
        capsuleActor->attachShape(*shape);
  
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*capsuleActor, mass);

        _gScene->addActor(*capsuleActor);

        return capsuleActor;
    }

    void InitializeCharacterController() {
        PxControllerManager* manager = PxCreateControllerManager(*_gScene);

        // Define the controller description
        physx::PxCapsuleControllerDesc desc;
        desc.height = 1.8f;          // Player height
        desc.radius = 0.3f;          // Capsule radius
        desc.material = _gPhysics->createMaterial(0.5f, 0.5f, 0.0f); // Friction/restitution
        desc.position = PxExtendedVec3(0, 5, 0); // Start position
        desc.slopeLimit = 0.707f;    // Max slope the player can walk up
        desc.stepOffset = 0.5f;      // Max step height
        desc.upDirection = PxVec3(0, 1, 0); // "Up" direction

       _gController = manager->createController(desc);
    }

    void MovePlayerController(const glm::vec3& direction, float deltaTime) {

        g_ControllerVerticalVelocity -= 9.81f * deltaTime; // Reset gravity when grounded
        

        physx::PxVec3 displacement(direction.x, g_ControllerVerticalVelocity, direction.z);
       PxControllerCollisionFlags flags = _gController->move(displacement, 0.0f, deltaTime, nullptr);

       if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
           g_ControllerVerticalVelocity = 0.0f;
       }
    }

    void UpdatePlayerControllerVerticalVelocity() {
        float jumpVelocity = 0.5f; 
        g_ControllerVerticalVelocity = jumpVelocity;
    }

    PxExtendedVec3 GetPlayerControllerPosition() {
        return _gController->getPosition();
    }

    void CreateCharacterActor(glm::vec3 position, float height, float mass) {
      physx::PxVec3 pxPosition(position.x, position.y, position.z);

      _playerActor = Physics::CreateDynamicCapsule(pxPosition, height / 2.0f, 0.25f, mass);
     /* _playerActor = Physics::CreateDynamicBox(pxPosition, PxVec3(0.5f, height / 2.0, 0.5), mass);*/

      _playerActor->setLinearDamping(1.0f);
      _playerActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
      _playerActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
      _playerActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
    }

    void CharacterActorJump() {
        float jumpImpulse = 200.25f;
        PxVec3 jumpForce(0.0f, jumpImpulse, 0.0f);
        _playerActor->addForce(jumpForce, physx::PxForceMode::eIMPULSE);
    }

    void MoveCharacterActor(glm::vec3 targetVelocity) {
        PxVec3 currentVelocity = _playerActor->getLinearVelocity();

        targetVelocity.y = currentVelocity.y;
        physx::PxVec3 pxTargetVelocity(targetVelocity.x, targetVelocity.y, targetVelocity.z);
        _playerActor->setLinearVelocity(pxTargetVelocity);
    }

    PxTransform GetCharacterActorPosition() {
        return _playerActor->getGlobalPose();
    }

    PxScene* GetScene() {
        return _gScene;
    }


    void Simulate(double deltaTime) {
        const double fixedTimestep = 1.0 / 420; // 420 fps
        static double accumulatedTime = 0.0;

        accumulatedTime += deltaTime;

        while (accumulatedTime >= fixedTimestep) {
            _gScene->simulate(fixedTimestep);
            _gScene->fetchResults(true);
            accumulatedTime -= fixedTimestep;
        }
    }

    PhysicsTransformData GetTransformFromPhysics(const physx::PxRigidActor* actor) {
        PhysicsTransformData transformData;

        if (!actor) return transformData;

        const physx::PxTransform& transform = actor->getGlobalPose();
        transformData.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
        transformData.rotation = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

        return transformData;
    }

    void CleanupPhysX() {
        _cubeActor->release();
        _gController->release();
        _gScene->release();
        _gDispatcher->release();
        _gMaterial->release();
        _gPhysics->release();
        _gFoundation->release();
    }
}