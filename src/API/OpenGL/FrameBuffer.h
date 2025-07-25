#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <iostream>
#include <vector>

#include "./GL_Utils.h"

struct ColorAttachment {
	const char* name = "undefined";
	GLuint textureID = 0;
	GLenum attachment = 0;
	GLenum internalFormat = 0;
	bool isMultisampled = false;
};

class FrameBuffer {
private:
	std::vector<ColorAttachment> m_colorAttachments;

	unsigned int m_depthTextureID = 0;
	unsigned int m_fbo = 0;
	unsigned int m_rbo = 0;

	unsigned int m_width = 0;
	unsigned int m_height = 0;

	// 2, 4, 8 16, 32 
	unsigned int m_msaaSamples = 4;

public:
	FrameBuffer() = default;
	FrameBuffer(unsigned int width, unsigned int height);
	void Create(unsigned int width, unsigned int height);
	void CreateAttachment(const char* name, GLenum internalFormat);
	void CreateMSAAAttachment(const char* name);
	void CreateDepthAttachment();
	void CreateDepthTextureAttachment();
	void DrawBuffers();
	void DrawBuffer();
	void Unbind();
	void Bind();
	void BindTextures();
	void SetViewport();
	void Resize(unsigned int width, unsigned int height);
	void ResizeMSAA(unsigned int width, unsigned int height);
	void ResizeRefraction(unsigned int width, unsigned int height);
	void Cleanup();
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetFBO() const;
	GLuint GetColorAttachmentByIndex(int index); 
	GLuint GetColorAttachmentTextureIdByIndex(int index);
	GLuint GetDepthTextureAttachmentId();
};