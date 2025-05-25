#pragma once

#include "../Common/CreateInfo.hpp"

class GameObject {
private:
	std::string m_name;
	std::string m_modelName;
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::mat4 m_rotation;
	GameObjectCreateInfo m_createInfo;
public:
	GameObject() = default;
	GameObject(GameObjectCreateInfo& createInfo);

	void SetPosition(glm::vec3 position);
	void SetRotation(glm::mat4 rotation);

	glm::vec3 GetPosition() const;
	glm::vec3 GetSize() const;
	glm::mat4 GetRotation() const;
	std::string GetModelName() const;
	std::string GetName() const;
};