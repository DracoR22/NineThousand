#pragma once

#include <glad/glad.h>
#include <cstddef>

class ShadowMap {
private:
	unsigned int m_depthMapFBO = 0;
	unsigned int m_depthTexture = 0;
	const unsigned int m_SHADOW_WIDTH = 1024, m_SHADOW_HEIGHT = 1024;
public:
	void Init();
	void Cleanup();
	void Unbind();
	void Clear();
};