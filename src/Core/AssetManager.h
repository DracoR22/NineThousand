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
#include "../Animations/Animator.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace AssetManager {
	inline std::vector<Model> g_models;
	inline std::vector<Texture> g_textures;
	inline std::vector<Animation> g_animations;
	inline std::vector<Animator> g_animators;

	// models
	void LoadAssimpModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo);
	void LoadModel(const std::string& name, ModelType type, ModelCreateInfo& createInfo);

	void DrawModel(const std::string& name, Shader& shader);

    Model* GetModelByName(const std::string& name);

	void CleanupModels();

	// textures
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);

	// animations
	void LoadAnimation(const std::string& name, const std::string& path, Model* model);
	Animation* GetAnimationByName(const std::string& name);

	void LoadAnimator(const std::string& name, Animation* animation);
	Animator* GetAnimatorByName(const std::string& name);
}