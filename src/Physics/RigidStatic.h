#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;

class RigidStatic {
public:
	RigidStatic() = default;

	void SetPxRigidStatic(PxRigidStatic* rigidStatic, PxShape* shape);

	glm::vec3 GetCurrentPosition();
	glm::quat GetCurrentRotation();

	PxRigidStatic* GetPxRigidStatic();
	PxShape* GetPxShape() { return m_pxShape; };

private:
	PxRigidStatic* m_pxRigidStatic = nullptr;
	PxShape* m_pxShape = nullptr;
};
