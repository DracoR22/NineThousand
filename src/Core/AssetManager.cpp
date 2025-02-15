#include "AssetManager.h"

namespace AssetManager {
	std::unordered_map<std::string, int> g_modelIndexMap;
	std::unordered_map<std::string, int> g_animationIndexMap;

	// ---------------------------------------------------------// MODELS //---------------------------------------------------------------------------//
	void LoadPrimitiveModel(const std::string& name) {

	}

	void LoadModel(const std::string& name, const std::string& path) {
		/*Model newModel = { name, {} };

		Assimp::Importer import;

        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        AssetManager::ProcessNode(scene->mRootNode, scene, newModel);*/

		Model model(name, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.05f));
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

		//glm::mat4 model = glm::mat4(1.0f);

		//model = glm::translate(model, existingModel->pos);
		//model *= existingModel->rotation;
		//model = glm::scale(model, existingModel->size);

		///*model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/

		//shader.setMat4("model", model);

		//for (unsigned int i = 0; i < existingModel->meshes.size(); i++)
		//	existingModel->meshes[i].draw(shader);
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
		/*Animation newAnimation(path, model);

		g_animations.push_back(newAnimation);
		g_animationIndexMap[name] = g_animations.size() - 1;*/

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