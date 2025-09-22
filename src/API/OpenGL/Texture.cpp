#include "Texture.h"

#include <iostream>

Texture::Texture(std::string dir, std::string path, aiTextureType type)
	: m_file(path), m_path(dir), m_type(type) {
	Generate();
}

void Texture::Generate() {
	glGenTextures(1, &m_id);
}

void Texture::AllocateMemory(TextureData& texData) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, texData.internalFormat, texData.width, texData.height, 0, texData.format, GL_UNSIGNED_BYTE, texData.pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::AllocateMemoryENVMap(TextureData& texData) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texData.width, texData.height, 0, GL_RGB, GL_FLOAT, texData.floatPixels.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::AllocateMemoryEXR(TextureData& texData) {
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, texData.internalFormat, texData.width, texData.height, 0, texData.format, GL_FLOAT, texData.floatPixels.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::Bind() {
	glBindTexture(GL_TEXTURE_2D, m_id);
}