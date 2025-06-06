#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

using namespace physx;

class RigidStatic {
private:
	PxRigidStatic* m_pxRigidStatic = nullptr;
public:
	RigidStatic() = default;
	~RigidStatic();

	void SetPxRigidStatic(PxRigidStatic* rigidStatic);

	PxRigidStatic* GetPxRigidStatic();
};
