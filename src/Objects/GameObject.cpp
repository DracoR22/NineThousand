#include "./GameObject.h"


GameObject::GameObject(GameObjectCreateInfo& createInfo) {
	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotationMatrix = createInfo.rotation;
	m_eulerRotation = glm::vec3(0.0f);
}

void GameObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void GameObject::SetRotationMatrix(glm::mat4 rotation) {
	m_rotationMatrix = rotation;
}

void GameObject::SetSize(glm::vec3 size) {
	m_size = size;
}

glm::vec3 GameObject::GetPosition() const {
	return m_position;
}

glm::vec3 GameObject::GetSize() const {
	return m_size;
}

glm::mat4 GameObject::GetRotationMatrix() const {
	return m_rotationMatrix;
}

void GameObject::SetRotationEuler(const glm::vec3& eulerDegrees) {
	m_eulerRotation = eulerDegrees;
	glm::vec3 radians = glm::radians(eulerDegrees);
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
}

glm::vec3 GameObject::GetRotationEuler() const {
	return m_eulerRotation;
}

std::string GameObject::GetModelName() const {
	return m_modelName;
}

std::string GameObject::GetName() const {
	return m_name;
}