#include "./PickUpObject.h"

PickUpObject::PickUpObject(PickUpObjectCreateInfo& createInfo) {
	m_position = createInfo.position;
	m_eulerRotation = createInfo.rotation;
	m_size = createInfo.size;
}

void PickUpObject::Update(double deltaTime) {
	const float rotationSpeed = 90.0f;

	m_eulerRotation.y += rotationSpeed * deltaTime;

	// avoid float overflow
	if (m_eulerRotation.y >= 360.0f) {
		m_eulerRotation.y -= 360.0f;
	}
}

void PickUpObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void PickUpObject::SetRotationEuler(const glm::vec3& eulerDegrees) {
	m_eulerRotation = eulerDegrees;
}

void PickUpObject::SetSize(glm::vec3 size) {
	m_size = size;
}

glm::mat4 PickUpObject::GetModelMatrix() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);
	model *= glm::mat4_cast(glm::quat(glm::radians(m_eulerRotation)));
	model = glm::scale(model, m_size);

	return model;
}