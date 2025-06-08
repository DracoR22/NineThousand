#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;

class RigidStatic {
private:
	PxRigidStatic* m_pxRigidStatic = nullptr;
public:
	RigidStatic() = default;
	~RigidStatic();

	void SetPxRigidStatic(PxRigidStatic* rigidStatic);

	glm::vec3 GetCurrentPosition();
	glm::quat GetCurrentRotation();

	PxRigidStatic* GetPxRigidStatic();
};
