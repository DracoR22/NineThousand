#include "Texture.h"

#include <iostream>

Texture::Texture() {}

Texture::Texture(std::string dir, std::string path, aiTextureType type)
	: dir(dir), path(path), type(type) {
	generate();
}

void Texture::generate() {
	glGenTextures(1, &id);
}

void Texture::load(bool flip) {
	stbi_set_flip_vertically_on_load(flip);

	int width, height, nChannels;

	unsigned char* data = stbi_load((dir + "/" + path).c_str(), &width, &height, &nChannels, 0);
	/*std::cout << "texture path: " << path << std::endl;*/

	GLenum internalFormat = GL_RGB; 
	GLenum format = GL_RGB;

	bool isNormal = (type == aiTextureType_NORMALS || type == aiTextureType_HEIGHT);

	switch (nChannels) {
	case 1:
		internalFormat = GL_RED;
		format = GL_RED;
		break;
	case 3:
		internalFormat = isNormal ? GL_RGB : GL_SRGB;
		format = GL_RGB;
		break;
	case 4:
		internalFormat = isNormal ? GL_RGBA : GL_SRGB_ALPHA;
		format = GL_RGBA;
		break;
	}

	if (data) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		std::cout << "Failed to load texture: " << (dir + "/" + path) << std::endl;
		std::cout << "STB Image failure reason: " << stbi_failure_reason() << std::endl;
	}

	stbi_image_free(data);
}

void Texture::AllocateTexture(TextureData& texData) {
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, texData.internalFormat, texData.width, texData.height, 0, texData.format, GL_UNSIGNED_BYTE, texData.pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}