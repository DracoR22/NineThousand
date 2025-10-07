#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../Common/Types.hpp"
#include "../Animation/Animator.hpp"
#include "../Common/Enums.hpp"
#include "../Common/Types.hpp"

using namespace physx;

class RigidDynamic {
public:
	RigidDynamic() = default;

	void SetPxRigidDynamic(PxRigidDynamic* rigidDynamic);
	void UpdateMassAndInertia(float mass);
	void SetGlobalPoseFromAnimator(Animator* animator, glm::mat4 modelMatrix);
	void SetUserData(PhysicsUserData userData);

	void ActivatePhysics();

	glm::vec3 GetCurrentPosition();
	glm::quat GetCurrentRotation();
	glm::mat4 GetCurrentRotationMatrix();

	PxRigidDynamic* GetPxRigidDynamic();

	void MarkForRemoval();
	bool IsMarkedForRemoval() const { return m_markedForRemoval; };

	glm::vec3 m_previousPosition;
	glm::mat4 m_previousRotation;
private:
	PxRigidDynamic* m_pxRigidDynamic = nullptr;
	bool m_markedForRemoval = false;
};
