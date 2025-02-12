#include "AssetManager.h"

namespace AssetManager {
	std::unordered_map<std::string, int> g_modelIndexMap;

	// ---------------------------------------------------------// MODELS //---------------------------------------------------------------------------//
	void LoadModel(const std::string& name, const std::string& path) {
		ModelData newModel = { name, {} };

		Assimp::Importer import;

        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }

        AssetManager::ProcessNode(scene->mRootNode, scene, newModel.meshes);

		g_models.push_back(newModel);
	    g_modelIndexMap[name] = g_models.size() - 1;
	}

    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& modelMeshes) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            modelMeshes.push_back(ProcessMesh(mesh, scene)); 
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene, modelMeshes);
        }
    }

    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			// position
			vertex.Position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);

			// normals
			vertex.Normal = glm::vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);

			// texture coords
			if (mesh->mTextureCoords[0]) {
				vertex.TexCoords = glm::vec2(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				);
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f);
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
	
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// diffuse maps
			std::vector<Texture> diffuseMaps = AssetManager::LoadTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			// specular maps
			std::vector<Texture> specularMaps = AssetManager::LoadTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		/*ExtractBoneWeightForVertices(vertices, mesh, scene);*/

		return Mesh(vertices, indices, textures);
    }

	void DrawModel(const std::string& name, Shader& shader) {
		ModelData* existingModel = AssetManager::GetModelByName(name);

		if (!existingModel) {
			std::cerr << "ERROR::DrawModel::Model '" << name << "' not found!" << std::endl;
			return;
		}

		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, existingModel->position);
		model *= existingModel->rotation;
		model = glm::scale(model, existingModel->size);

		/*model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/

		shader.setMat4("model", model);

		for (unsigned int i = 0; i < existingModel->meshes.size(); i++)
			existingModel->meshes[i].draw(shader);
	}

	ModelData* GetModelByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			int index = it->second;
			return &g_models[index];
		}

		return nullptr;
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
}