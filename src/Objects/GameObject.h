#pragma once

#include "../Common/CreateInfo.hpp"
#include "../Common/Types.hpp"
#include "../Physics/Physics.h"

#include <glm/gtx/quaternion.hpp>

class GameObject {
private:
	std::string m_name;
	std::string m_modelName;
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::mat4 m_rotationMatrix;
	glm::vec3 m_eulerRotation;
	glm::vec2 m_textureScale;
	GameObjectCreateInfo m_createInfo;
	bool m_selected;
	std::vector<MeshRenderingInfo> m_meshRenderingInfo;

	uint64_t m_physicsId = 0;
public:
	GameObject() = default;
	GameObject(GameObjectCreateInfo& createInfo);

	void SetPosition(glm::vec3 position);
	void SetRotationMatrix(glm::mat4 rotation);
	void SetRotationEuler(const glm::vec3& eulerDegrees);
	void SetSize(glm::vec3 size);
	void SetTextureScale(glm::vec2 scale);

	glm::vec3 GetPosition() const;
	glm::vec3 GetSize() const;
	glm::mat4 GetRotationMatrix() const;
	glm::vec3 GetRotationEuler() const;
	glm::mat4 GetModelMatrix() const;

	std::string GetModelName() const;
	std::string GetName() const;
	glm::vec2 GetTextureScale() const;
	uint64_t GetPhysicsId() const { return m_physicsId; };
	std::vector<MeshRenderingInfo> GetMeshRenderingInfo() { return m_meshRenderingInfo; };

	void PushToMeshRenderingInfo(const std::string& meshName, const std::string& materialName);
	void SetMeshRenderingMaterialByMeshName(const std::string& meshName, const std::string& materialName);

	bool IsSelected() const;
	void SetSelected(bool select);

	GameObjectCreateInfo GetCreateInfo() const;
	GameObjectCreateInfo GetLatestCreateInfo() const;
};