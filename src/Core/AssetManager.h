#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Graphics/Mesh.h"
#include "../Graphics/Texture.h"

#include "../Animations/AssimpGlmHelpers.hpp"
#include "../Animations/Animation.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct BoneInfo {
	int id;
	glm::mat4 offset;
};

struct ModelData {
	const std::string name;

	std::vector<Mesh> meshes;
	glm::vec3 position = glm::vec3(0.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::vec3 size = glm::vec3(0.05f);

	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;
};

namespace AssetManager {
	inline std::vector<ModelData> g_models;
	inline std::vector<Texture> g_textures;
	inline std::vector<Animation> g_animations;

	// models
	void LoadModel(const std::string& name, const std::string& path);
	void LoadPrimitiveModel(const std::string& name);

	void ProcessNode(aiNode* node, const aiScene* scene, ModelData& model);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, ModelData& model);
	void DrawModel(const std::string& name, Shader& shader);

    ModelData* GetModelByName(const std::string& name);

	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, ModelData& model);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void SetVertexBoneDataToDefault(Vertex& vertex);

	/*std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();*/

	void CleanupModels();

	// textures
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);

	// animations
	void LoadAnimation(const std::string& name, const std::string& path, ModelData* model);
	Animation* GetAnimationByName(const std::string& name);
}