#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "../Graphics/Model.h"

namespace Physics {
    using namespace physx;

    void InitPhysx();

    /**
    * @brief Creates a dynamic cube actor in the PhysX simulation.
    *
    * @param position The initial position of the cube in the simulation (world space coordinates).
    * @param halfExtents The half-dimensions of the cube along each axis (width/2, height/2, depth/2).
    * @param mass The mass of the cube, which affects its motion and interaction with other objects in the simulation.
    * @return A pointer to the created `PxRigidDynamic` cube actor.
    *
    */
    PxRigidDynamic* CreateDynamicActor(const PxVec3& position, const PxVec3& halfExtents, PxReal mass);

    PxRigidStatic* CreateStaticActor(const PxVec3& position, const PxVec3& halfExtents);
  
    void Simulate(double dt);
    void UpdateModelFromPhysics(Model& model, const physx::PxRigidActor* actor);

    void CleanupPhysX();
}