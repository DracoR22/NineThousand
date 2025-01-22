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

    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass);
    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass);

    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents);
  
    void Simulate(double dt);

    PhysicsTransformData GetTransformFromPhysics(const physx::PxRigidActor* actor);

    void CleanupPhysX();
}