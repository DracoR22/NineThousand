#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>
#include <vector>
#include <iostream>

#include "Shader.h"

class CubeMap {
public:
	CubeMap();

	void loadTextures(std::vector<std::string> faces);
	void render(Shader shader, glm::mat4 viewMatrix, glm::mat4 projection);
	void init();
	void cleanup();

	void FlipImageHorizontally(unsigned char* data, int width, int height, int channels);

	unsigned int getId() const;

private:
	unsigned int id;
	unsigned int VAO, VBO;
	/*std::vector<std::string> faces;*/
};