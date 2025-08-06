#pragma once

#include <glad/glad.h>
#include <cstddef>
#include <iostream>

class ShadowMap {
private:
	unsigned int m_depthMapFBO = 0;
	unsigned int m_depthTexture = 0;
	const unsigned int m_depthMapResolution = 1024;
public:
	ShadowMap() = default;
	void Init();
	void InitCubeMap();
	void InitCSM(int shadowCascadeCount);
	void Cleanup();
	void Unbind();
	void Clear();
	unsigned int GetTextureID() const { return m_depthTexture; };
};