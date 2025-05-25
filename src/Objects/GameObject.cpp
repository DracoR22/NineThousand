#include "./GameObject.h"

GameObject::GameObject(GameObjectCreateInfo& createInfo) {
	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotation = createInfo.rotation;
}

void GameObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void GameObject::SetRotation(glm::mat4 rotation) {
	m_rotation = rotation;
}

glm::vec3 GameObject::GetPosition() const {
	return m_position;
}

glm::vec3 GameObject::GetSize() const {
	return m_size;
}

glm::mat4 GameObject::GetRotation() const {
	return m_rotation;
}

std::string GameObject::GetModelName() const {
	return m_modelName;
}

std::string GameObject::GetName() const {
	return m_name;
}