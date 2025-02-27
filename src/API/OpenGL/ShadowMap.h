#pragma once

#include <glad/glad.h>
#include <cstddef>

struct ShadowMap {
	void Init();
	void Cleanup();
	void Unbind();
	void Clear();

	unsigned int m_depthMapFBO = 0;
	unsigned int m_depthTexture = 0;
	const unsigned int m_SHADOW_WIDTH = 2048, m_SHADOW_HEIGHT = 2048;
};