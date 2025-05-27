#pragma once

#include "../Common/CreateInfo.hpp"

#include <glm/gtx/quaternion.hpp>

class GameObject {
private:
	std::string m_name;
	std::string m_modelName;
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::mat4 m_rotationMatrix;
	glm::vec3 m_eulerRotation;
	GameObjectCreateInfo m_createInfo;
public:
	GameObject() = default;
	GameObject(GameObjectCreateInfo& createInfo);

	void SetPosition(glm::vec3 position);
	void SetRotationMatrix(glm::mat4 rotation);
	void SetRotationEuler(const glm::vec3& eulerDegrees);
	void SetSize(glm::vec3 size);

	glm::vec3 GetPosition() const;
	glm::vec3 GetSize() const;
	glm::mat4 GetRotationMatrix() const;
	glm::vec3 GetRotationEuler() const;
	std::string GetModelName() const;
	std::string GetName() const;
};