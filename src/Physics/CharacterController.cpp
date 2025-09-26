#include "./CharacterController.h"
#include <iostream>

CharacterController::~CharacterController() {
	if (m_pxController) {
		m_pxController->release();
		m_pxController = nullptr;
	}
}

void CharacterController::SetPxController(PxController* pxController) {
	m_pxController = pxController;
}

void CharacterController::SetVerticalVelocity(float jumpVelocity) {
	m_verticalVelocity = jumpVelocity;
}

void CharacterController::Move(const glm::vec3& direction) {
	double fixedDeltaTime = 1.0 / 60;

	m_verticalVelocity -= 0.81f * fixedDeltaTime;

	physx::PxVec3 displacement(direction.x, m_verticalVelocity, direction.z);
	PxControllerCollisionFlags flags = m_pxController->move(displacement, 0.001f, fixedDeltaTime, nullptr);

	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
		m_verticalVelocity = 0.0f;
	}
}

void CharacterController::SetPosition(glm::vec3 position) {
	m_pxController->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

PxController* CharacterController::GetPxController() {
	if (m_pxController) {
		return m_pxController;
	}
	else {
		std::cout << "Error: CharacterController::GetPxController(): m_pxController was not set!\n";
		return nullptr;
	}
}