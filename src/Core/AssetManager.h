#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <future>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../API/OpenGL/Mesh.h"
#include "../API/OpenGL/Texture.h"

#include "../Animation/Animation.hpp"
#include "../Animation/Animator.hpp"

#include "../Common/CreateInfo.hpp"
#include "../Common/Types.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace AssetManager {
	inline std::vector<Model> g_models;
	inline std::vector<Texture> g_textures;
	inline std::vector<Material> g_materials;
	inline std::vector<Animation> g_animations;
	inline std::vector<Animator> g_animators;
	inline std::vector<Mesh> g_meshes;

	// models
	void LoadAssimpModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo);
	void LoadModel(const std::string& name, ModelType type, ModelCreateInfo& createInfo);
	void DrawModel(const std::string& name, Shader& shader);
	void DrawModelInstanced(const std::string& name, Shader& shader, std::vector<glm::vec3> offsets);
    Model* GetModelByName(const std::string& name);
	void ComputeModelAABB(Model& model);
	std::vector<Model>& GetModels();
	void CleanupModels();

	// meshes
	Mesh* GetMeshByName(const std::string& name);
	Mesh* GetMeshByIndex(int index);
	int GetMeshIndexByName(const std::string& name);

	// textures
	TextureData DecodeTexture(const std::string& dir, const std::string& name, aiTextureType type);
	void LoadTexture(const std::string& name, aiTextureType type);
	void LoadAllTexturesAsync();
	aiTextureType GetFileTextureType(const std::string& filename);
	aiTextureType GetFileTextureType(const std::string& filename);
	Texture* GetTextureByName(const std::string& name);
	Texture* GetTextureByIndex(int index);
	int GetTextureIndexByName(const std::string& name, bool ignoreWarning = false);

	// materials
	void BuildMaterials();
	std::string GetMaterialNameFromTextureFile(const std::string& filePath);
	Material* GetDefaultMaterial();
	Material* GetMaterialByName(const std::string& name);
	Material* GetMaterialByIndex(int index);
	std::vector<Material>& GetAllMaterials();
	int GetMaterialIndexByName(const std::string& name);

	// animations
	void LoadAnimation(const std::string& name, const std::string& path, Model* model);
	Animation* GetAnimationByName(const std::string& name);
	void LoadAnimator(const std::string& name, Animation* animation);
	Animator* GetAnimatorByName(const std::string& name);
}