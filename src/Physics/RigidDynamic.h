#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;

class RigidDynamic {
public:
	RigidDynamic() = default;
	~RigidDynamic();

	void SetPxRigidDynamic(PxRigidDynamic* rigidDynamic);
	void UpdateMassAndInertia(float mass);

	glm::vec3 GetCurrentPosition();
	glm::quat GetCurrentRotation();
	glm::mat4 GetCurrentRotationMatrix();

	PxRigidDynamic* GetPxRigidDynamic();

	glm::vec3 m_previousPosition;
	glm::mat4 m_previousRotation;
private:
	PxRigidDynamic* m_pxRigidDynamic = nullptr;
};
