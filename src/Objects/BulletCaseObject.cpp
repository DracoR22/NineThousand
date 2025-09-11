#include "./BulletCaseObject.h"
#include <glm/gtx/quaternion.hpp>

BulletCaseObject::BulletCaseObject(BulletCaseCreateInfo& createInfo) {
	glm::vec3 radians = glm::radians(createInfo.rotation);

	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
	m_eulerRotation = createInfo.rotation;
	m_materialIndex = createInfo.materialIndex;
	m_physicsId = createInfo.physicsId;
}

void BulletCaseObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void BulletCaseObject::SetRotationMatrix(glm::mat4 rotation) {
	m_rotationMatrix = rotation;

	// also update euler rotation
	glm::quat q = glm::quat_cast(rotation);
	m_eulerRotation = glm::degrees(glm::eulerAngles(q));
}

void BulletCaseObject::SetRotationEuler(const glm::vec3& eulerDegrees) {
	m_eulerRotation = eulerDegrees;

	// Also update the rotation matrix
	glm::vec3 radians = glm::radians(eulerDegrees);
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
}

void BulletCaseObject::SetSize(glm::vec3 size) {
	m_size = size;
}

glm::mat4 BulletCaseObject::GetModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, m_position);
	model *= glm::toMat4(glm::quat(glm::radians(m_eulerRotation)));
	model = glm::scale(model, m_size);

	return model;
}
