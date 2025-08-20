#pragma once

#include "../Common/CreateInfo.hpp"

class LightObject {
public:
	LightObject() = default;
	LightObject(LightCreateInfo& createInfo);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetColor() const { return m_color; };
	float GetStrength() const { return m_strength; };
	float GetRadius() const { return m_radius; };
	LightType GetLightType() const { return m_type; };

	void SetPosition(glm::vec3 position);
	void SetColor(glm::vec3 color);
	void SetStrength(float strength);
	void SetRadius(float radius);
	void SetLightType(LightType type);

private:
	glm::vec3 m_position;
	glm::vec3 m_color;

	float m_strength;
	float m_radius;

	LightType m_type;
};