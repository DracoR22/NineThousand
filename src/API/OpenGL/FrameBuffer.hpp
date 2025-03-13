#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <iostream>

struct FrameBuffer {
private:
	unsigned int m_fbo = 0;
	unsigned int m_rbo = 0;
	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;

public:
	void Create(unsigned int width, unsigned int height) {
		glGenFramebuffers(1, &m_fbo);
		this->m_width = width;
		this->m_height = height;
	}

	void CreateAttachment() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void BindTexture() {
		glBindTexture(GL_TEXTURE_2D, m_texture);
	}

	void SetViewport() {
		glViewport(0, 0, m_width, m_height);
	}

	unsigned int GetWidth() const { return m_width; }

	unsigned int GetHeight() const { return m_height; }

	void Resize(unsigned int width, unsigned int height) {
		// Store new width and height
		this->m_width = width;
		this->m_height = height;

		// Resize viewport
		glViewport(0, 0, m_width, m_height);

		// Resize color texture
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		// Resize renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	}

	void Cleanup() {
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_texture);
		glDeleteRenderbuffers(1, &m_rbo);
	}
};