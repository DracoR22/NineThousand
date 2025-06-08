#include "./WaterObject.h"

WaterObject::WaterObject(WaterObjectCreateInfo& createInfo) {
	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotation = createInfo.rotation;
	m_waterColor = createInfo.waterColor;
	m_waterTransparency = createInfo.waterTransparency;
}

void WaterObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void WaterObject::SetRotation(glm::mat4 rotation) {
	m_rotation = rotation;
}

glm::vec3 WaterObject::GetPosition() const {
	return m_position;
}

glm::vec3 WaterObject::GetSize() const {
	return m_size;
}

glm::mat4 WaterObject::GetRotation() const {
	return m_rotation;
}

std::string WaterObject::GetModelName() const {
	return m_modelName;
}

std::string WaterObject::GetName() const {
	return m_name;
}