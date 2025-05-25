#include "./WaterPlaneObject.h"

WaterPlaneObject::WaterPlaneObject(WaterPlaneObjectCreateInfo& createInfo) {
	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotation = createInfo.rotation;
	m_waterColor = createInfo.waterColor;
	m_waterTransparency = createInfo.waterTransparency;
}

void WaterPlaneObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void WaterPlaneObject::SetRotation(glm::mat4 rotation) {
	m_rotation = rotation;
}

glm::vec3 WaterPlaneObject::GetPosition() const {
	return m_position;
}

glm::vec3 WaterPlaneObject::GetSize() const {
	return m_size;
}

glm::mat4 WaterPlaneObject::GetRotation() const {
	return m_rotation;
}

std::string WaterPlaneObject::GetModelName() const {
	return m_modelName;
}

std::string WaterPlaneObject::GetName() const {
	return m_name;
}