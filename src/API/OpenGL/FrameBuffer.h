#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <iostream>

class FrameBuffer {
private:
	unsigned int m_fbo = 0;
	unsigned int m_rbo = 0;

	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;

	// 2, 4, 8 16, 32 TODO: let user choose this
	unsigned int m_msaaSamples = 8;

public:
	FrameBuffer() = default;
	FrameBuffer(unsigned int width, unsigned int height);
	void Create(unsigned int width, unsigned int height);
	void CreateAttachment();
	void CreateMSAAAttachment();
	void Bind();
	void BindTexture();
	void SetViewport();
	void Resize(unsigned int width, unsigned int height);
	void Cleanup();
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetFBO() const;

};