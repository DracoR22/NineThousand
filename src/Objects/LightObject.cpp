#include "./LightObject.h"

LightObject::LightObject(LightCreateInfo& createInfo) {
	m_position = createInfo.position;
	m_color = createInfo.color;
	m_radius = createInfo.radius;
	m_strength = createInfo.strength;
	m_type = createInfo.type;
}

void LightObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void LightObject::SetColor(glm::vec3 color) {
	m_color = color;
}
 
void LightObject::SetStrength(float strength) {
	m_strength = strength;
}

void LightObject::SetRadius(float radius) {
	m_radius = radius;
}

void LightObject::SetLightType(LightType type) {
	m_type = type;
}