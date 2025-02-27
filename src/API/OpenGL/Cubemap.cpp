#include "Cubemap.h"

CubeMap::CubeMap() {}

void CubeMap::loadTextures(std::vector<std::string> faces) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	int width, height, nrChannels;

	unsigned char* data;
	for (unsigned int i = 0; i < 6; i++) {
		data = stbi_load((faces[i]).c_str(), &width, &height, &nrChannels, 0);

		GLenum colorMode = GL_RED;

	/*	stbi_set_flip_vertically_on_load(false);*/

		if (data) {
			GLenum colorMode = (nrChannels == 4) ? GL_RGBA : GL_RGB;

			// Workaround to flip SkyRight horizontically due to texture being inverted
			if (i == 0) { // SkyRight is the first texture in the list tho
				FlipImageHorizontally(data, width, height, nrChannels);
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Failed to load texture at " << faces[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::init() {
	float skyboxVertices[] = {
		// positions          
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

void CubeMap::render(Shader shader, glm::mat4 viewMatrix, glm::mat4 projection) {
	glDepthFunc(GL_LEQUAL);
	shader.activate();

	// TODO: Change the Camera class and projection usage. Myabe create a Scene class or make it part of the Player class
	glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));

	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void CubeMap::cleanup() {
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