#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "../Core/Model.h"
#include "./RigidStatic.h"
#include "./RigidDynamic.h"
#include "./CharacterController.h"
#include "../Utils/Utils.h"

struct PhysicsTransformData {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

namespace Physics {
    using namespace physx;

    void Init();
    void Simulate(double dt);

    // Create
    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass);
    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass);
    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents);

    PhysicsTransformData GetActorTransform(const physx::PxRigidActor* actor);

    // Character Controller
    uint64_t CreateCharacterController(const glm::vec3& position, float height);
    void MoveCharacterController(uint64_t physicsId, const glm::vec3& direction);
    void UpdateCharacterControllerVerticalVelocity(uint64_t physicsId);
    glm::vec3 GetCharacterControllerPosition(uint64_t physicsId);
    CharacterController* GetCharacterControllerById(uint64_t physicsId);

    // Rigid Dynamics
    uint64_t CreateRigidDynamicBox(PhysicsTransformData transform, const PxVec3& halfExtents, PxReal mass);
    RigidDynamic* GetRigidDynamicById(uint64_t id);
    uint64_t CreateRigidDynamicConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform, float mass, const glm::vec3& scale);

    // Rigid Statics
    uint64_t CreateRigidStaticBox(PhysicsTransformData transform, const PxVec3& halfExtents);
    uint64_t CreateRigidStaticConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform);
    RigidStatic* GetRigidStaticById(uint64_t id);
    std::unordered_map<uint64_t, RigidStatic>& GetRigidStaticsMap();

    glm::vec3 PxVec3toGlmVec3(PxVec3 vec);
    glm::quat PxQuatToGlmQuat(PxQuat quat);

    PxScene* GetScene();

    double GetInterpolationAlpha();
    void CleanupPhysX();
}