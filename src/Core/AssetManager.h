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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ModelData {
	const std::string name;
	std::vector<Mesh> meshes;
	glm::vec3 position = glm::vec3(0.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::vec3 size = glm::vec3(0.05f);
};

namespace AssetManager {
	inline std::vector<ModelData> g_models;
	inline std::vector<Texture> g_textures;

	// models
	void LoadModel(const std::string& name, const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& modelMeshes);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void DrawModel(const std::string& name, Shader& shader);

    ModelData* GetModelByName(const std::string& name);
	/*void SetModelTransform(const std::string& name, const std::string& path);*/

	// textures
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);
}