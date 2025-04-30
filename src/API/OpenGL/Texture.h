#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <assimp/scene.h>

struct TextureData {
	int width = 0;
	int height = 0;
	int channels = 0;
	GLenum internalFormat = GL_RGB;
	GLenum format = GL_RGB;
	std::vector<unsigned char> pixels = {};
	std::string name = "unedfined";
	aiTextureType type = aiTextureType_NONE;
};

class Texture {
public: 
	Texture();
	Texture(std::string dir, std::string path, aiTextureType type);

	void generate();
	void load(bool flip = true);
	void AllocateTexture(TextureData& texData);

	void bind();

	// texture object
	unsigned int id;
	aiTextureType type;
	std::string dir;
	std::string path;
};