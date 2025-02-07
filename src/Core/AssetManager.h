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

namespace AssetManager {
	void LoadModel(std::string& name, std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& modelMeshes);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);
}