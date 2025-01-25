#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "../Graphics/Model.h"

struct PhysicsTransformData {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

namespace Physics {
    using namespace physx;

    void InitPhysx();

    /* Collision Shapes */
    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass);
    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass);

    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents);

    PhysicsTransformData GetTransformFromPhysics(const physx::PxRigidActor* actor);

    /* Character Controller */
    void InitializeCharacterController();
    void MovePlayerController(const glm::vec3& direction, float deltaTime);
    PxExtendedVec3 GetPlayerControllerPosition();

    /* Charcter Actors */
    void CreateCharacterActor(glm::vec3 position, float height, float mass);
    void CharacterActorJump();
    void MoveCharacterActor(glm::vec3 targetVelocity);
    PxTransform GetCharacterActorPosition();

    /* Utility Functions */
    void Simulate(double dt);
    void CleanupPhysX();
}