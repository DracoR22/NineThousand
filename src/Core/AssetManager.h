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
#include "../Graphics/Primitives.hpp"

#include "../Animations/Animation.hpp"



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
	inline std::vector<Model> g_models;
	inline std::vector<Texture> g_textures;
	inline std::vector<Animation> g_animations;

	// models
	void LoadModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo);
	void LoadPrimitiveModel(const std::string& name, PrimitiveModel::Type, ModelCreateInfo& createInfo);

	void DrawModel(const std::string& name, Shader& shader);

    Model* GetModelByName(const std::string& name);

	void CleanupModels();

	// textures
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);

	// animations
	void LoadAnimation(const std::string& name, const std::string& path, Model* model);
	Animation* GetAnimationByName(const std::string& name);
}