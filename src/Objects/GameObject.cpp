#include "./GameObject.h"
#include <glm/gtx/string_cast.hpp>
#include "../Core/AssetManager.h"

GameObject::GameObject(GameObjectCreateInfo& createInfo) {
	glm::vec3 radians = glm::radians(createInfo.rotation);

	std::cout << "Model Name" << createInfo.name << std::endl;

	m_name = createInfo.name;
	m_modelName = createInfo.modelName;
	m_position = createInfo.position;
	m_size = createInfo.size;
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
	m_eulerRotation = createInfo.rotation;
	m_createInfo = createInfo;
	m_textureScale = createInfo.textureScale;
	m_selected = false;

	// Set mesh rendering info
	Model* model = AssetManager::GetModelByName(m_modelName);

	if (model) {
		if (createInfo.meshRenderingInfo.empty()) { // always build meshRenderingInfo even if its not provided
			for (Mesh& mesh : model->m_meshes) {
				int meshIndex = AssetManager::GetMeshIndexByName(mesh.m_Name);
				int defaultMaterialIndex = AssetManager::GetMaterialIndexByName("Default");

				MeshRenderingInfo& info = m_meshRenderingInfo.emplace_back();
				info.meshIndex = meshIndex;
				info.materialIndex = defaultMaterialIndex;

				m_meshRenderingIndexMap[mesh.m_Name] = m_meshRenderingInfo.size() - 1;
			}
		}
		else { // use the create info if provided
			for (MeshRenderingInfo& info : createInfo.meshRenderingInfo) {
				Mesh* mesh = AssetManager::GetMeshByIndex(info.meshIndex);

				MeshRenderingInfo& newInfo = m_meshRenderingInfo.emplace_back();
				newInfo.meshIndex = info.meshIndex;
				newInfo.materialIndex = info.materialIndex;

				m_meshRenderingIndexMap[mesh->m_Name] = m_meshRenderingInfo.size() - 1;
			}
		}
	}

	// Physics
	if (createInfo.modelName == "Cube" && createInfo.name != "Cube0" || createInfo.name == "Plane2" || createInfo.name == "Plane3" || createInfo.name == "Plane6") {
		PhysicsTransformData physicsTransformData;
		physicsTransformData.position = createInfo.position;
		physicsTransformData.rotation = Utils::GlmVec3ToGlmQuat(createInfo.rotation);

		uint64_t physicsId = Physics::CreateRigidStaticBox(
			physicsTransformData,
		    PxVec3(
			createInfo.size.x * 0.5f,
			createInfo.size.y * 0.5f,
			createInfo.size.z * 0.5f
			)
		);

		m_physicsId = physicsId;
	}

	if (createInfo.name == "Cube0") {
		PhysicsTransformData physicsTransformData;
		physicsTransformData.position = createInfo.position;
		physicsTransformData.rotation = Utils::GlmVec3ToGlmQuat(createInfo.rotation);

		/*std::cout << "CUBE POSITION " << glm::to_string(createInfo.position) << std::endl;*/

		uint64_t physicsId = Physics::CreateRigidDynamicBox(
			physicsTransformData,
			PxVec3(
				createInfo.size.x * 0.5f,
				createInfo.size.y * 0.5f,
				createInfo.size.z * 0.5f
			),
			10.0f
		);

		m_physicsId = physicsId;
	}
}

void GameObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void GameObject::SetRotationMatrix(glm::mat4 rotation) {
	m_rotationMatrix = rotation;

	// also update euler rotation
	glm::quat q = glm::quat_cast(rotation);
	m_eulerRotation = glm::degrees(glm::eulerAngles(q));
}

void GameObject::SetSize(glm::vec3 size) {
	m_size = size;
}

void GameObject::SetTextureScale(glm::vec2 scale) {
	m_textureScale = scale;
}

glm::vec3 GameObject::GetPosition() const {
	return m_position;
}

glm::vec3 GameObject::GetSize() const {
	return m_size;
}

glm::mat4 GameObject::GetRotationMatrix() const {
	return m_rotationMatrix;
}

glm::vec3 GameObject::GetRotationEuler() const {
	return m_eulerRotation;
}

glm::mat4 GameObject::GetModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, m_position);
	model *= m_rotationMatrix;
	model = glm::scale(model, m_size);

	return model;
}

void GameObject::SetRotationEuler(const glm::vec3& eulerDegrees) {
	m_eulerRotation = eulerDegrees;

	// Also update the rotation matrix
	glm::vec3 radians = glm::radians(eulerDegrees);
	m_rotationMatrix = glm::toMat4(glm::quat(radians));
}

std::string GameObject::GetModelName() const {
	return m_modelName;
}

std::string GameObject::GetName() const {
	return m_name;
}

glm::vec2 GameObject::GetTextureScale() const {
	return m_textureScale;
}

int GameObject::GetMeshMaterialIndex(const std::string& meshName) {
	auto it = m_meshRenderingIndexMap.find(meshName);
	if (it != m_meshRenderingIndexMap.end()) {
		size_t infoIndex = it->second;
		return m_meshRenderingInfo[infoIndex].materialIndex;
	}

	std::cout << "GameObject::GetMeshMaterialIndex() failed: mesh '" << meshName << "' not found.\n";
	return -1; 
}

void GameObject::SetMeshMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
	int meshIndex = AssetManager::GetMeshIndexByName(meshName);
	int materialIndex = AssetManager::GetMaterialIndexByName(materialName);

	if (meshIndex == -1) {
		std::cerr << "GameObject::SetMeshRenderingMaterialByMeshName() failed: mesh '" << meshName << "' not found.\n";
		return;
	}

	if (materialIndex == -1) {
		std::cerr << "GameObject::SetMeshRenderingMaterialByMeshName() failed: material '" << materialName << "' not found.\n";
		return;
	}


	for (auto& info : m_meshRenderingInfo) {
		if (info.meshIndex == meshIndex) {
			info.materialIndex = materialIndex;
			return;
		}
	}

	std::cerr << "GameObject::SetMeshRenderingMaterialByMeshName() failed: mesh '" << meshName << "' not found in m_meshRenderingInfo.\n";
}

bool GameObject::IsSelected() const {
	return m_selected;
}

void GameObject::SetSelected(bool select) {
	m_selected = select;
}

GameObjectCreateInfo GameObject::GetCreateInfo() const {
	return m_createInfo;
}

GameObjectCreateInfo GameObject::GetLatestCreateInfo() const {
	GameObjectCreateInfo info;
	info.name = m_name;
	info.modelName = m_modelName;
	info.position = m_position;
	info.size = m_size;
	info.rotation = m_eulerRotation;
	info.textureScale = m_textureScale;
	info.meshRenderingInfo = m_meshRenderingInfo;
	return info;
}