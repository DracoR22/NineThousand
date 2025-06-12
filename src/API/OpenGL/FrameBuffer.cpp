#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height) {
	Create(width, height);
}

void FrameBuffer::Create(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &m_fbo);
	this->m_width = width;
	this->m_height = height;
}

void FrameBuffer::CreateAttachment(const char* name, GLenum internalFormat) {
	ColorAttachment& colorAttachment = m_colorAttachments.emplace_back();
	colorAttachment.name = name;
	colorAttachment.attachment = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(m_colorAttachments.size() - 1);
	colorAttachment.internalFormat = internalFormat;

	GLenum format = OpenGLUtils::GLInternalFormatToGLFormat(internalFormat);
	GLenum type = OpenGLUtils::GLInternalFormatToGLType(internalFormat);

	glGenTextures(1, &colorAttachment.textureID);
	glBindTexture(GL_TEXTURE_2D, colorAttachment.textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment.attachment, GL_TEXTURE_2D, colorAttachment.textureID, 0);
}

void FrameBuffer::CreateMSAAAttachment(const char* name) {
	ColorAttachment& colorAttachment = m_colorAttachments.emplace_back();
	colorAttachment.name = name;
	colorAttachment.attachment = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(m_colorAttachments.size() - 1);
	colorAttachment.isMultisampled = true;

	glGenTextures(1, &colorAttachment.textureID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttachment.textureID);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_msaaSamples, GL_RGB, m_width, m_height, GL_TRUE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment.attachment, GL_TEXTURE_2D_MULTISAMPLE, colorAttachment.textureID, 0);

	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msaaSamples, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: MSAA Framebuffer is not complete!" << std::endl;
}

void FrameBuffer::CreateDepthAttachment() {
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void FrameBuffer::CreateDepthTextureAttachment() {
	glGenTextures(1, &m_depthTextureID);
	glBindTexture(GL_TEXTURE_2D, m_depthTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0);
}

void FrameBuffer::DrawBuffers() {
	std::vector<GLenum> attachments;
	for (int i = 0; i < m_colorAttachments.size(); i++) {
		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
	}
	glDrawBuffers(attachments.size(), attachments.data());
}

void FrameBuffer::DrawBuffer() {
   glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::BindTextures() {
	for (size_t i = 0; i < m_colorAttachments.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i].textureID);
	}
}

void FrameBuffer::SetViewport() {
	glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::Resize(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;

	glViewport(0, 0, m_width, m_height);

	for (auto& attachment : m_colorAttachments) {
		glBindTexture(GL_TEXTURE_2D, attachment.textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
}

void FrameBuffer::ResizeMSAA(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;

	for (auto& attachment : m_colorAttachments) {
		if (!attachment.isMultisampled) continue;

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, attachment.textureID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_msaaSamples, GL_RGB, m_width, m_height, GL_TRUE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment.attachment, GL_TEXTURE_2D_MULTISAMPLE, attachment.textureID, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_msaaSamples, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::MSAA Resize failed! Incomplete framebuffer." << std::endl;
}

void FrameBuffer::Cleanup() {
	glDeleteFramebuffers(1, &m_fbo);
	for (auto& attachment : m_colorAttachments) {
		glDeleteTextures(1, &attachment.textureID);
	}
	glDeleteRenderbuffers(1, &m_rbo);
}

unsigned int FrameBuffer::GetWidth() const {
	return m_width;
}

unsigned int FrameBuffer::GetHeight() const {
	return m_height;
}

unsigned int FrameBuffer::GetFBO() const {
	return m_fbo;
}

GLuint FrameBuffer::GetColorAttachmentByIndex(int index) {
	return m_colorAttachments[index].attachment;
}

GLuint FrameBuffer::GetColorAttachmentTextureIdByIndex(int index) {
	return m_colorAttachments[index].textureID;
}

GLuint FrameBuffer::GetDepthTextureAttachmentId() {
	return m_depthTextureID;
}
