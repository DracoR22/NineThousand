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
	float far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	std::vector<LightCreateInfo> sceneLights = GetSceneLights();
	glm::vec3 lightPos = sceneLights[0].position;

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	/*glViewport(0, 0, Window::currentWidth, Window::currentHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);*/
	/*g_renderData.cubeMapShadowMap.Clear();
	g_shaders.shadowCubeMapShader.activate();
	for (unsigned int i = 0; i < 6; ++i)
		g_shaders.shadowCubeMapShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	g_shaders.shadowCubeMapShader.setFloat("farPlane", far);
	g_shaders.shadowCubeMapShader.setVec3("lightPos", lightPos);
	AssetManager::DrawModel("Cube", g_shaders.shadowCubeMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	glm::mat4 lightView = glm::mat4(1.0f);

	if (!sceneLights.empty()) {
		lightView = glm::lookAt(sceneLights[0].position, Scene::GetGameObjectByName("Cube0")->GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
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