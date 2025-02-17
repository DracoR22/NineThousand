#include "AssetManager.h"

namespace AssetManager {
	std::unordered_map<std::string, int> g_modelIndexMap;
	std::unordered_map<std::string, int> g_animationIndexMap;

	// ---------------------------------------------------------// MODELS //---------------------------------------------------------------------------//
	void LoadPrimitiveModel(const std::string& name, PrimitiveModel::Type type, ModelCreateInfo& createInfo) {
		PrimitiveModel primitiveModel(name, type, createInfo);

		g_models.push_back(primitiveModel);
		g_modelIndexMap[name] = g_models.size() - 1;
	}

	void LoadModel(const std::string& name, const std::string& path, ModelCreateInfo& createInfo) {
		Model model(name, createInfo);
		model.loadModel(path);

		g_models.push_back(model);
	    g_modelIndexMap[name] = g_models.size() - 1;
	}

	void DrawModel(const std::string& name, Shader& shader) {
		Model* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cerr << "ERROR::DrawModel::Model '" << name << "' not found!" << std::endl;
			return;
		}

		existingModel->draw(shader);
	}

	Model* GetModelByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			int index = it->second;
			return &g_models[index];
		}

		return nullptr;
	}

	void CleanupModels() {
		for (auto& model : g_models) {
			for (auto& mesh : model.meshes) {
				mesh.cleanup();
			}
		}
		g_models.clear();
		g_modelIndexMap.clear();
		g_textures.clear();
	}

	// ---------------------------------------------------------// TEXTURES //---------------------------------------------------------------------------//
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			std::string fullPath = str.C_Str();
			std::string fileName = fullPath.substr(fullPath.find_last_of("/\\") + 1);
			std::cout << "textures: " << fileName << std::endl;

			// prevent duplicate loading
			bool skip = false;
			for (const auto& loadedTex : g_textures) {
				if (loadedTex.path == fileName) { // Compare file names
					textures.push_back(loadedTex);
					skip = true;
					break;
				}
			}

			if (!skip) {
				Texture tex("resources/textures", fileName, type);
				tex.load(false);
				textures.push_back(tex);
				g_textures.push_back(tex);
			}
		}
		return textures;
	}

// ---------------------------------------------------------// ANIMATIONS //---------------------------------------------------------------------------//
	void LoadAnimation(const std::string& name, const std::string& path, Model* model) {
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

		return nullptr;
	}
}