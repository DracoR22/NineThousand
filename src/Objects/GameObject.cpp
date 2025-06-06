#include "./GameObject.h"
#include <glm/gtx/string_cast.hpp>

GameObject::GameObject(GameObjectCreateInfo& createInfo) {
	glm::vec3 radians = glm::radians(createInfo.rotation);

	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
	m_eulerRotation = createInfo.rotation;
	m_createInfo = createInfo;
	m_textureScale = createInfo.textureScale;
	m_selected = false;

	if (createInfo.modelName == "Cube" && createInfo.name != "Cube0") {
		PhysicsTransformData physicsTransformData;
		physicsTransformData.position = createInfo.position;
		physicsTransformData.rotation = Utils::GlmVec3ToGlmQuat(createInfo.rotation);

		uint64_t physicsId = Physics::CreateRigidStaticBox(
			physicsTransformData,
		    PxVec3(createInfo.size.x * 0.5f,
			createInfo.size.y * 0.5f,
			createInfo.size.z * 0.5f)
		);

		m_physicsId = physicsId;
	}
}

void GameObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void GameObject::SetRotationMatrix(glm::mat4 rotation) {
	m_rotationMatrix = rotation;

	// also update euler rotation
	glm::quat q = glm::quat_cast(rotation);
	m_eulerRotation = glm::degrees(glm::eulerAngles(q));
}

void GameObject::SetSize(glm::vec3 size) {
	m_size = size;
}

void GameObject::SetTextureScale(float scale) {
	m_textureScale = scale;
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

	// Also update the rotation matrix
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

float GameObject::GetTextureScale() const {
	return m_textureScale;
}

bool GameObject::IsSelected() const {
	return m_selected;
}

void GameObject::SetSelected(bool select) {
	m_selected = select;
}

GameObjectCreateInfo GameObject::GetCreateInfo() const {
	return m_createInfo;
}

GameObjectCreateInfo GameObject::GetLatestCreateInfo() const {
	GameObjectCreateInfo info;
	info.name = m_name;
	info.modelName = m_modelName;
	info.position = m_position;
	info.size = m_size;
	info.rotation = m_eulerRotation;
	info.textureScale = m_textureScale;
	return info;
}