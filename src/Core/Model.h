#pragma once

#include <vector>
#include <string>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Graphics/Mesh.h"
#include "../Graphics/Texture.h"

#include "../Animations/AssimpGlmHelpers.hpp"

#include "CreateInfo.hpp"

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
public:
	Model(const std::string& name, const ModelCreateInfo& createInfo);

	void draw(Shader& shader);

	glm::vec3 pos;
	glm::vec3 size;
	glm::mat4 rotation = glm::mat4(1.0f);

	void setPosition(const glm::vec3& newPos);
	void setRotation(const glm::mat4& newRotation);
	void setSize(const glm::vec3& newSize);

	std::vector<Mesh> meshes;

	void loadAssimpModel(std::string path);
	void LoadModel(ModelType type);

	void cleanup();

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();

	const std::string& GetName();

private:
	std::string directory;
	std::vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);

	std::string m_name;

	// for animations
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;

	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};