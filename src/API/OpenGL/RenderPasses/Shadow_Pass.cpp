#include "../Renderer.h"

void OpenGLRenderer::ShadowPass() {
	ShadowMap& shadowMap = GetShadowMap();
	Shader* shadowMapShader = GetShaderByName("ShadowMap");

	if (shadowMapShader == nullptr) {
		std::cerr << "OpenGLRenderer::ShadowPass() error: shader is nullptr" << std::endl;
		return;
	}

	float aspect = static_cast<float>(Window::currentWidth) / static_cast<float>(Window::currentHeight);
	float near = 1.0f;
	float far = 7.5f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	std::vector<LightCreateInfo> sceneLights = GetSceneLights();
	glm::vec3 lightPos = sceneLights[0].position;

	glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	glm::mat4 lightView = glm::mat4(1.0f);

	if (!sceneLights.empty()) {

		lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 lightProjection = orthogonalProjection * lightView;

	shadowMap.Clear();

	glCullFace(GL_FRONT);

	shadowMapShader->activate();
	shadowMapShader->setMat4("lightProjection", lightProjection);

	shadowMapShader->setMat4("model", Scene::GetGameObjectByName("Cube0")->GetModelMatrix());
	AssetManager::DrawModel("Cube", *shadowMapShader);

	glCullFace(GL_BACK);

	shadowMap.Unbind();
	glViewport(0, 0, Window::currentWidth, Window::currentHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}