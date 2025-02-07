#include "AssetManager.h"

namespace AssetManager {
    std::string g_directory = "resources/textures";
    std::vector<Mesh> g_meshes;
    std::unordered_map<std::string, std::vector<Mesh>> g_models;
    std::unordered_map<std::string, std::string> g_directories;
	std::vector<Texture> g_texturesLoaded;

	void LoadModel(std::string& name, std::string& path) {
        g_models[name].clear();
        g_directories[name] = path.substr(0, path.find_last_of('/'));

		Assimp::Importer import;

        const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }
        /* directory = path.substr(0, path.find_last_of('/'));*/

        AssetManager::ProcessNode(scene->mRootNode, scene, g_models[name]);
	}

    void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& modelMeshes) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            modelMeshes.push_back(ProcessMesh(mesh, scene)); // Store meshes for this model
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

		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		// process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			//if (noTex) {
			//	// diffuse color
			//	aiColor4D diff(1.0f);
			//	aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);

			//	// specular color
			//	aiColor4D spec(1.0f);
			//	aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

			//	return Mesh(br, vertices, indices, diff, spec);
			//}

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
			for (const auto& loadedTex : g_texturesLoaded) {
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
				g_texturesLoaded.push_back(tex);
			}
		}
		return textures;
	}
}