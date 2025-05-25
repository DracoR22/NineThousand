#include "Mesh.h"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices) {
	std::vector<Vertex> ret(noVertices);

	/*int stride = sizeof(Vertex) / sizeof(float);*/

	int stride = 8;

	for (int i = 0; i < noVertices; i++) {
		ret[i].m_Position = glm::vec3(
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2]
		);

		ret[i].m_Normal = glm::vec3(
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5]
		);

		ret[i].m_TexCoords = glm::vec2(
			vertices[i * stride + 6],
			vertices[i * stride + 7]
		);
	}

	return ret;
}

  void Vertex::CalcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices) {
	unsigned char* counts = (unsigned char*)malloc(list.size() * sizeof(unsigned char));
	for (unsigned int i = 0, len = list.size(); i < len; i++) {
		counts[i] = 0;
	}

	// iterate through indices and calculate vectors for each face
	for (unsigned int i = 0, len = indices.size(); i < len; i += 3) {
		// 3 vertices corresponding to the face
		Vertex v1 = list[indices[i + 0]];
		Vertex v2 = list[indices[i + 1]];
		Vertex v3 = list[indices[i + 2]];

		// calculate edges
		glm::vec3 edge1 = v2.m_Position - v1.m_Position;
		glm::vec3 edge2 = v3.m_Position - v1.m_Position;

		// calculate dUVs
		glm::vec2 deltaUV1 = v2.m_TexCoords - v1.m_TexCoords;
		glm::vec2 deltaUV2 = v3.m_TexCoords - v1.m_TexCoords;

		// use inverse of the UV matrix to determine tangent
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = {
			f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
		};

		// average in the new tangent vector
		Utils::AverageVectors(list[indices[i + 0]].m_Tangent, tangent, counts[indices[i + 0]]++);
		Utils::AverageVectors(list[indices[i + 1]].m_Tangent, tangent, counts[indices[i + 1]]++);
		Utils::AverageVectors(list[indices[i + 2]].m_Tangent, tangent, counts[indices[i + 2]]++);
	}
}


Mesh::Mesh(const std::string& name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->m_Name = name;
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	SetupMesh();
}

	void Mesh::SetupInstance() {
		glBindVertexArray(m_VAO);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

void Mesh::SetupMesh() {
	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Normal));

	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_TexCoords));

	// Tangent Vector
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Tangent));

	// Bone Ids
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	// Bone Weights
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader, unsigned int instances) {
	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string name;
		switch (textures[i].type) {
		case aiTextureType_DIFFUSE:
			name = "baseTexture";
			break;
		case aiTextureType_NORMALS:
			name = "normalTexture";
			break;
		case aiTextureType_SPECULAR:
			name = "rmaTexture";
			break;
		default:
			continue;
		}

		// set shader texture value
		shader.setInt(name, i);

		textures[i].bind();
	}

	// draw mesh
	glBindVertexArray(m_VAO);
	if (instances > 0) {
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instances);
	}
	else {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < textures.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, 0); 
	}
}

void Mesh::Cleanup() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}