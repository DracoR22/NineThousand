#pragma once

#include "../Common/CreateInfo.hpp"
#include "../Core/AssetManager.h"
#include <glm/gtx/quaternion.hpp>
#include <unordered_map>

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

	PickUpType GetType() const { return m_pickUpType; };

	bool IsCollected() const { return m_isCollected; };
	void SetCollected();

	void SetMaterialByMeshName(const std::string meshName, const std::string& materialName);
	int GetMeshMaterialIndex(const std::string& meshName);
private:
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::vec3 m_eulerRotation;

	bool m_isCollected;

	PickUpType m_pickUpType;

	// mesh stuff
	std::vector<MeshRenderingInfo> m_meshRenderingInfo;
	std::unordered_map<std::string, size_t> m_meshRenderingIndexMap;
};