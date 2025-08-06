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
	std::vector<float> floatPixels = {}; // for hdr images
	std::string name = "undefined";
	aiTextureType type = aiTextureType_NONE;
};

class Texture {
public: 
	Texture() = default;
	Texture(std::string dir, std::string path, aiTextureType type);
	void Generate();
	void AllocateMemory(TextureData& texData);
	void AllocateMemoryENVMap(TextureData& texData);
	void Bind();

public:
	unsigned int m_id;
	aiTextureType m_type;
	std::string m_file;
	std::string m_path;
	int m_width = 0;
	int m_height = 0;
	int m_format = 0;
	int m_internalFormat = 0;
	int m_numChannels = 0;
};