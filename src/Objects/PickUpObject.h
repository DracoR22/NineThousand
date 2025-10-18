#pragma once

#include "../Common/CreateInfo.hpp"
#include <glm/gtx/quaternion.hpp>

class PickUpObject {
public:
	PickUpObject() = default;
	PickUpObject(PickUpObjectCreateInfo& createInfo);

	void Update(double deltaTime);

	void SetPosition(glm::vec3 position);
	void SetRotationEuler(const glm::vec3& eulerDegrees);
	void SetSize(glm::vec3 size);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetSize() const { return m_size; };
	glm::vec3 GetRotationEuler() const { return m_eulerRotation;  };
	glm::mat4 GetModelMatrix() const;

private:
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::vec3 m_eulerRotation;
};