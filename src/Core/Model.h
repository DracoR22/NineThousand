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



struct BoneInfo
{
	int id;
	glm::mat4 offset;
};

enum class ModelType {
	CUBE,
	PLANE
};

class Model {
private:
	std::string m_directory;
	std::vector<Texture> m_textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadDefaultMaterials(aiMaterial* mat, aiTextureType type);
	void LoadRMAMaterials(const std::string& meshName, aiMaterial* material, std::vector<Texture>& textures);

	std::string m_name;

	// animations
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	unsigned int m_instanceOffsetVBO;

	glm::vec3 m_aabbMin = glm::vec3(0);
	glm::vec3 m_aabbMax = glm::vec3(0);;
public:
	std::vector<Mesh> meshes;
public:
	Model(const std::string& name, const ModelCreateInfo& createInfo);

	void draw(Shader& shader);
	void DrawInstanced(Shader& shader, std::vector<glm::vec3> offsets);

	void loadAssimpModel(std::string path);
	void LoadModel(ModelType type, ModelCreateInfo& createInfo);

	void CreateInstanceBuffers();

	void cleanup();

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();

	const std::string& GetName();

	void SetAABBMin(glm::vec3 aabbMin);
	void SetAABBMax(glm::vec3 aabbMax);
	glm::vec3 GetAABBMin() const { return m_aabbMin; }
	glm::vec3 GetAABBMax() const { return m_aabbMax; }
};