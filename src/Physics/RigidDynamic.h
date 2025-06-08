#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;

class RigidDynamic {
private:
	PxRigidDynamic* m_pxRigidDynamic = nullptr;
public:
	RigidDynamic() = default;
	~RigidDynamic();

	void SetPxRigidDynamic(PxRigidDynamic* rigidDynamic);
	void UpdateMassAndInertia(float mass);

	glm::vec3 GetCurrentPosition();
	glm::quat GetCurrentRotation();

	PxRigidDynamic* GetPxRigidDynamic();
};
