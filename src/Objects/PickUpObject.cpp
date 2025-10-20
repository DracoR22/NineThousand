#include "./PickUpObject.h"

PickUpObject::PickUpObject(PickUpObjectCreateInfo& createInfo) {
	m_position = createInfo.position;
	m_eulerRotation = createInfo.rotation;
	m_size = createInfo.size;
	m_pickUpType = createInfo.pickUpType;

	if (m_pickUpType == PickUpType::AKS74U) {
		Model* model = AssetManager::GetModelByName("AKS74U");

		SetMaterialByMeshName("AKS74UHandGuard", "AKS74U_0");
		SetMaterialByMeshName("AKS74UBolt", "AKS74U_1");
		SetMaterialByMeshName("AKS74UPistolGrip", "AKS74U_2");
		SetMaterialByMeshName("AKS74UMag", "AKS74U_3");
		SetMaterialByMeshName("AKS74UBarrel", "AKS74U_4");
		SetMaterialByMeshName("AKS74UReceiver", "AKS74U_1");
		SetMaterialByMeshName("AKS74UFollower", "AKS74U_1");
	}
}

void PickUpObject::Update(double deltaTime) {
	const float rotationSpeed = 90.0f;

	m_eulerRotation.y += rotationSpeed * deltaTime;

	// avoid float overflow
	if (m_eulerRotation.y >= 360.0f) {
		m_eulerRotation.y -= 360.0f;
	}
}

void PickUpObject::SetPosition(glm::vec3 position) {
	m_position = position;
}

void PickUpObject::SetRotationEuler(const glm::vec3& eulerDegrees) {
	m_eulerRotation = eulerDegrees;
}

void PickUpObject::SetSize(glm::vec3 size) {
	m_size = size;
}

glm::mat4 PickUpObject::GetModelMatrix() const {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);
	model *= glm::mat4_cast(glm::quat(glm::radians(m_eulerRotation)));
	model = glm::scale(model, m_size);

	return model;
}

void PickUpObject::SetMaterialByMeshName(const std::string meshName, const std::string& materialName) {
	MeshRenderingInfo& info = m_meshRenderingInfo.emplace_back();
	info.meshIndex = AssetManager::GetMeshIndexByName(meshName);
	info.materialIndex = AssetManager::GetMaterialIndexByName(materialName);

	m_meshRenderingIndexMap[meshName] = m_meshRenderingInfo.size() - 1;
}

int PickUpObject::GetMeshMaterialIndex(const std::string& meshName) {
	auto it = m_meshRenderingIndexMap.find(meshName);
	if (it != m_meshRenderingIndexMap.end()) {
		size_t infoIndex = it->second;
		return m_meshRenderingInfo[infoIndex].materialIndex;
	}

	std::cout << "PickUpObject::GetMeshMaterialIndex() failed: mesh '" << meshName << "' not found.\n";
	return -1;
}