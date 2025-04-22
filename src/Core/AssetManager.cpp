#include "AssetManager.h"

namespace AssetManager {
	std::unordered_map<std::string, int> g_modelIndexMap;
	std::unordered_map<std::string, int> g_animationIndexMap;
	std::unordered_map<std::string, int> g_animatorIndexMap;
	std::unordered_map<std::string, int> g_textureIndexMap;

	// ---------------------------------------------------------// MODELS //---------------------------------------------------------------------------//
	void LoadModel(const std::string& name, ModelType type, ModelCreateInfo& createInfo) {
		Model model(name, createInfo);
		model.LoadModel(type, createInfo);

		g_models.push_back(model);
		g_modelIndexMap[name] = g_models.size() - 1;
	}

	void LoadAssimpModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo) {
		Model model(name, createInfo);
		model.loadAssimpModel(path);

		g_models.push_back(model);
	    g_modelIndexMap[name] = g_models.size() - 1;
	}

	void DrawModel(const std::string& name, Shader& shader) {
		Model* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cout << "AssetManager::DrawModel() failed because '" << name << "' does not exist!\n";
			return;
		}

		existingModel->draw(shader);
	}

	void DrawModelInstanced(const std::string& name, Shader& shader, std::vector<glm::vec3> offsets) {
		Model* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cout << "AssetManager::DrawModel() failed because '" << name << "' does not exist!\n";
			return;
		}

		existingModel->DrawInstanced(shader, offsets);
	}

	Model* GetModelByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			int index = it->second;
			return &g_models[index];
		}

		std::cout << "AssetManager::GetModelByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	void CleanupModels() {
		for (auto& model : g_models) {
			model.cleanup();
		}
		g_models.clear();
		g_modelIndexMap.clear();
		g_textures.clear();
	}

	// ---------------------------------------------------------// TEXTURES //---------------------------------------------------------------------------//
	void LoadTexture(const std::string& name, aiTextureType type) {
		Texture texture("resources/textures", name, type);
		
		g_textures.push_back(texture);
		g_textureIndexMap[name] = g_textureIndexMap.size() - 1;
	}

	Texture* GetTextureByName(const std::string& name) {
		auto it = g_textureIndexMap.find(name);
		if (it != g_textureIndexMap.end()) {
			int index = it->second;
			return &g_textures[index];
		}

		std::cout << "AssetManager::GetTextureByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

// ---------------------------------------------------------// ANIMATIONS //---------------------------------------------------------------------------//
	void LoadAnimation(const std::string& name, const std::string& path, Model* model) {
		static bool reserved = false;
		if (!reserved) {
			g_animations.reserve(20);  
			reserved = true;
		}

		Animation animation(path, model);
		g_animations.push_back(animation);
		g_animationIndexMap[name] = g_animations.size() - 1;
	}

	Animation* GetAnimationByName(const std::string& name) {
		auto it = g_animationIndexMap.find(name);
		if (it != g_animationIndexMap.end()) {
			int index = it->second;
			return &g_animations[index];
		}

		std::cout << "AssetManager::GetAnimationByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}

	void LoadAnimator(const std::string& name, Animation* animation) {
		static bool reserved = false;
		if (!reserved) {
			g_animators.reserve(20);
			reserved = true;
		}

		Animator animator(animation);
		g_animators.push_back(animator);
		g_animatorIndexMap[name] = g_animators.size() - 1;
	}

	Animator* GetAnimatorByName(const std::string& name) {
		auto it = g_animatorIndexMap.find(name);
		if (it != g_animatorIndexMap.end()) {
			int index = it->second;
			return &g_animators[index];
		}

		std::cout << "AssetManager::GetAnimatorByName() failed because '" << name << "' does not exist!\n";
		return nullptr;
	}
}