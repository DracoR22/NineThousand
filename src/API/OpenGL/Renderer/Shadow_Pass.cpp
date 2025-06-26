#include "./Renderer.h"

void OpenGLRenderer::ShadowPass() {
	ShadowMap* csmDepth = GetShadowMapByName("CSM");
	Shader* csmDepthShader = GetShaderByName("CSM");
	UBO* csmDepthUBO = GetUBOByName("CSM");

	if (csmDepthShader == nullptr) {
		std::cerr << "OpenGLRenderer::ShadowPass() error: shader is nullptr" << std::endl;
		return;
	}

	Camera* camera = CameraManager::GetActiveCamera();
	std::vector<float>& shadowCascadeLevels = GetShadowCascadeLevels();

	const auto lightMatrices = Utils::GetLightSpaceMatrices(camera->GetNearPlane(), camera->GetFarPlane(), shadowCascadeLevels, (float)Window::m_windowWidth, (float)Window::m_windowHeight, camera->getZoom(), camera->GetViewMatrix());

	csmDepthUBO->UpdateDataArray(lightMatrices);

	csmDepth->Clear();
	glCullFace(GL_FRONT);
	csmDepthShader->activate();
	csmDepthShader->setMat4("model", Scene::GetGameObjectByName("Cube0")->GetModelMatrix());
	AssetManager::DrawModel("Cube", *csmDepthShader);
	glCullFace(GL_BACK);
	csmDepth->Unbind();

	glViewport(0, 0, GetRenderResolution().x, GetRenderResolution().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}