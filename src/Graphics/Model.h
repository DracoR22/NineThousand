#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Model {
public:
	Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));

	void draw(Shader& shader);

	glm::vec3 pos;
	glm::vec3 size;

	std::vector<Mesh> meshes;

	void loadModel(std::string path);

	void cleanup();
private:
	std::string directory;
	std::vector<Texture> textures_loaded;
	
	void processNode(aiNode* node, const aiScene* scene);
	
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type);
};