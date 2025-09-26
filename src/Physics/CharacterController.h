#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace physx;

class CharacterController {
public:
	CharacterController() = default;
	~CharacterController();

	void SetPxController(PxController* pxController);
	void SetVerticalVelocity(float jumpVelocity);
	void Move(const glm::vec3& direction);
	void SetPosition(glm::vec3 position);

	PxController* GetPxController();
private:
	PxController* m_pxController;
	float m_verticalVelocity = 0.0f;
};