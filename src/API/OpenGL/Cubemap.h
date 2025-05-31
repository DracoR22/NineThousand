#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <string>
#include <vector>
#include <iostream>

#include "Shader.h"
#include "Texture.h"

class CubeMap {
public:
	CubeMap() = default;
	void LoadTextures(std::vector<Texture>& faceTextures);
	void Init();
	void Draw(Shader shader, glm::mat4 viewMatrix, glm::mat4 projection);
	void Cleanup();
	void FlipImageHorizontally(unsigned char* data, int width, int height, int channels);
	unsigned int getId() const;
private:
	unsigned int id;
	unsigned int VAO, VBO;
};