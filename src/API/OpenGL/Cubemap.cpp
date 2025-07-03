#include "Cubemap.h"


void CubeMap::LoadTextures(std::vector<Texture>& faceTextures) {
	if (faceTextures.size() != 6) {
		std::cerr << "Cubemap requires exactly 6 textures.\n";
		return;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (unsigned int i = 0; i < 6; ++i) {
		const Texture& tex = faceTextures[i];
		glBindTexture(GL_TEXTURE_2D, tex.m_id);

		// interpret the format as linear, I dont know if this is correct since I originally uploaded it as srgb from the asset manager
		GLenum externalFormat = (tex.m_format == GL_RGBA) ? GL_RGBA : GL_RGB;
		GLenum internalFormat = externalFormat; 

		int numChannels = (externalFormat == GL_RGBA) ? 4 : 3;

		std::vector<unsigned char> pixels(tex.m_width * tex.m_height * numChannels);
		glGetTexImage(GL_TEXTURE_2D, 0, externalFormat, GL_UNSIGNED_BYTE, pixels.data());
		
		// I flip this one manually
		if (tex.m_file == "SkyRight.jpg") {
			FlipImageHorizontally(pixels.data(), tex.m_width, tex.m_height, numChannels);
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, tex.m_width, tex.m_height, 0, externalFormat, GL_UNSIGNED_BYTE, pixels.data());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::Init() {
	float skyboxVertices[] = {         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	// vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void CubeMap::Draw(Shader shader, glm::mat4 viewMatrix, glm::mat4 projection) {
	glDepthFunc(GL_LEQUAL);
	shader.activate();

	glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));

	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void CubeMap::Cleanup() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteTextures(1, &id);
}

void CubeMap::FlipImageHorizontally(unsigned char* data, int width, int height, int channels) {
	int rowSize = width * channels;
	unsigned char* tempRow = new unsigned char[rowSize];

	for (int y = 0; y < height; y++) {
		unsigned char* rowStart = data + (y * rowSize);
		for (int x = 0; x < width / 2; x++) {
			for (int c = 0; c < channels; c++) {
				std::swap(rowStart[x * channels + c], rowStart[(width - 1 - x) * channels + c]);
			}
		}
	}

	delete[] tempRow;
}

unsigned int CubeMap::getId() const {
	return id;
}