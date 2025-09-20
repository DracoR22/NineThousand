#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "../Core/Model.h"
#include "./RigidStatic.h"
#include "./RigidDynamic.h"
#include "./CharacterController.h"
#include "../Utils/Utils.h"
#include "../Common/Enums.hpp"

struct PhysicsTransformData {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

struct PhysicsUserData {
    uint64_t objectId;
    uint64_t physicsId;
    PhysicsType physicsType;
    ObjectType objectType;
};

struct PhysicsRayResult {
    PhysicsUserData userData;
    glm::vec3 hitPosition;
    glm::vec3 hitNormal;
    glm::vec3 rayDirection;
    bool hitFound;
};

namespace Physics {
    using namespace physx;

    void Init();
    void BeginFrame();
    void Simulate(double dt);

    // Create
    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass);
    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass);
    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents);

    PhysicsTransformData GetActorTransform(const physx::PxRigidActor* actor);

    // Character Controller
    uint64_t CreateCharacterController(uint64_t objectId, const glm::vec3& position, float height, ObjectType objectType);
    void MoveCharacterController(uint64_t physicsId, const glm::vec3& direction);
    void UpdateCharacterControllerVerticalVelocity(uint64_t physicsId);
    glm::vec3 GetCharacterControllerPosition(uint64_t physicsId);
    CharacterController* GetCharacterControllerById(uint64_t physicsId);
    std::unordered_map<uint64_t, CharacterController>& GetCharacterControllers();

    // Rigid Dynamics
    RigidDynamic* GetRigidDynamicById(uint64_t id);
    uint64_t CreateRigidDynamicBox(PhysicsTransformData transform, const glm::vec3& halfExtents, PxReal mass, const glm::vec3 initialForce, const glm::vec3 initialTorque);
    uint64_t CreateRigidDynamicConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform, float mass, const glm::vec3& scale, const glm::vec3 initialForce, const glm::vec3 initialTorque);
    void RemoveRigidDynamic(uint64_t id);
    void MarkRigidDynamicForRemoval(uint64_t id);
    void SetRigidDynamicGlobalPose(uint64_t id, glm::mat4 transformMatrix);
    void AddForceToRigidDynamic(uint64_t id, glm::vec3 direction, float strength);

    // Rigid Statics
    uint64_t CreateRigidStaticBox(PhysicsTransformData transform, const PxVec3& halfExtents);
    uint64_t CreateRigidStaticConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform);
    RigidStatic* GetRigidStaticById(uint64_t id);
    std::unordered_map<uint64_t, RigidStatic>& GetRigidStaticsMap();
    void SetRigidStaticGlobalPose(uint64_t id, glm::mat4 transformMatrix);

    PhysicsRayResult CastPhysXRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength);
    glm::vec3 PxVec3toGlmVec3(PxVec3 vec);
    glm::quat PxQuatToGlmQuat(PxQuat quat);
    PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix);

    PxScene* GetScene();

    double GetInterpolationAlpha();
    void CleanupPhysX();
}