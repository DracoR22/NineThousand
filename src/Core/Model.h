#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../API/OpenGL/Mesh.h"
#include "../API/OpenGL/Texture.h"

#include "../Animation/AssimpGlmHelpers.hpp"

#include "../Common/CreateInfo.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct BoneInfo {
	int id;
	glm::mat4 offset;
};

enum class ModelType {
	CUBE,
	PLANE
};

class Model {
public:
	std::vector<Mesh> m_meshes;
public:
	Model(const std::string& name);

	void LoadSkinnedModel(std::string path);
	void LoadModel(ModelType type, std::vector<glm::vec3> instanceOffsets = {});

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void DrawInstanced(Shader& shader, std::vector<glm::vec3> offsets);

	void CreateInstanceBuffers();

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();
	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	const std::string& GetName();

	void SetAABBMin(glm::vec3 aabbMin);
	void SetAABBMax(glm::vec3 aabbMax);
	glm::vec3 GetAABBMin() const { return m_aabbMin; }
	glm::vec3 GetAABBMax() const { return m_aabbMax; }

	std::vector<Texture> LoadDefaultMaterials(aiMaterial* mat, aiTextureType type);
	void Cleanup();
private:
	std::string m_name;
	std::string m_defaultTexturesDirectory;
	std::vector<Texture> m_defaultTextures;

	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	unsigned int m_instanceOffsetVBO = 0;

	glm::vec3 m_aabbMin = glm::vec3(0);
	glm::vec3 m_aabbMax = glm::vec3(0);
};