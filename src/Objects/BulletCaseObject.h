#pragma once

#include "../Common/CreateInfo.hpp"

class BulletCaseObject {
public:
	BulletCaseObject() = default;
	BulletCaseObject(BulletCaseCreateInfo& createInfo);

	void SetPosition(glm::vec3 position);
	void SetRotationMatrix(glm::mat4 rotation);
	void SetRotationEuler(const glm::vec3& eulerDegrees);
	void SetSize(glm::vec3 size);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetSize() const { return m_size; };
	glm::mat4 GetRotationMatrix() const { return m_rotationMatrix; };
	glm::vec3 GetRotationEuler() const { return m_eulerRotation; };
	glm::mat4 GetModelMatrix();

	uint64_t GetPhysicsId() const { return m_physicsId; };

	std::string GetModelName() const { return m_modelName; };
	int GetMaterialIndex() const { return m_materialIndex; };
private:
	std::string m_modelName;
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::mat4 m_rotationMatrix;
	glm::vec3 m_eulerRotation;

	uint64_t m_physicsId;

    int m_materialIndex;
	int m_isEmissive = false;
};