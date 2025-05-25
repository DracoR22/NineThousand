#pragma once

#include "Shader.h"
#include "Texture.h"

#include "../../Utils/Utils.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4
#define UPPER_BOUND 100

struct Vertex {
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec2 m_TexCoords;
	glm::vec3 m_Tangent;   

	int m_BoneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };
	float m_Weights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f };

	static std::vector<struct Vertex> genList(float* vertices, int noVertices);
	static void CalcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices);
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	std::vector<glm::vec3> instanceOffsets;
	std::string m_Name;
public:
	Mesh(const std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});

	void Draw(Shader& shader, unsigned int instances = 0);
	void Cleanup();
	void SetupInstance();
	void SetupMesh();
	void GetVAO();
private:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;
};

