#include "../Renderer.h"

void OpenGLRenderer::ShadowPass() {
	ShadowMap* csmDepth = GetShadowMapByName("CSM");
	Shader* csmDepthShader = GetShaderByName("CSM");

	if (csmDepthShader == nullptr) {
		std::cerr << "OpenGLRenderer::ShadowPass() error: shader is nullptr" << std::endl;
		return;
	}

	csmDepth->Clear();
	glCullFace(GL_FRONT);
	csmDepthShader->activate();
	csmDepthShader->setMat4("model", Scene::GetGameObjectByName("Cube0")->GetModelMatrix());
	AssetManager::DrawModel("Cube", *csmDepthShader);
	glCullFace(GL_BACK);
	csmDepth->Unbind();

	glViewport(0, 0, Window::currentWidth, Window::currentHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}