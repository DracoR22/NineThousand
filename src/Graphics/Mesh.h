#pragma once

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;

	static std::vector<struct Vertex> genList(float* vertices, int noVertices);
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// contructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});

	void draw(Shader& shader);

	void cleanup();
private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

