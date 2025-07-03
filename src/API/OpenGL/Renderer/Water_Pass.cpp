#include "./Renderer.h"

void OpenGLRenderer::PreWaterPass() {
	FrameBuffer* refractionFBO = GetFrameBufferByName("Refraction");
	ShadowMap* csmDepth = GetShadowMapByName("CSM");

	Shader* lightingShader = GetShaderByName("Lighting");

	Camera* camera = CameraManager::GetActiveCamera();
	std::vector<LightCreateInfo>& sceneLights = GetSceneLights();
	std::vector<float>& shadowCascadeLevels = GetShadowCascadeLevels();

	refractionFBO->Bind();
	glViewport(0, 0, Window::m_windowWidth, Window::m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightingShader->activate();
	lightingShader->setMat4("view", camera->GetViewMatrix());
	lightingShader->setMat4("projection", camera->GetProjectionMatrix());

	for (int i = 0; i < sceneLights.size(); i++) {
		std::string lightUniform = "lights[" + std::to_string(i) + "]";

		lightingShader->setFloat(lightUniform + ".posX", sceneLights[i].position.x);
		lightingShader->setFloat(lightUniform + ".posY", sceneLights[i].position.y);
		lightingShader->setFloat(lightUniform + ".posZ", sceneLights[i].position.z);
		lightingShader->setFloat(lightUniform + ".radius", sceneLights[i].radius);
		lightingShader->setFloat(lightUniform + ".strength", sceneLights[i].strength);
		lightingShader->setFloat(lightUniform + ".colorR", sceneLights[i].color.r);
		lightingShader->setFloat(lightUniform + ".colorG", sceneLights[i].color.g);
		lightingShader->setFloat(lightUniform + ".colorB", sceneLights[i].color.b);
		lightingShader->setInt(lightUniform + ".type", static_cast<int>(sceneLights[i].type));
	}
	lightingShader->setInt("noLights", sceneLights.size());
	lightingShader->set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);
	lightingShader->setVec3("lightDir", glm::normalize(glm::vec3(20.0f, 50, 20.0f)));
	lightingShader->setFloat("farPlane", camera->GetFarPlane());
	lightingShader->setInt("cascadeCount", shadowCascadeLevels.size());
	for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
	{
		lightingShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
	}
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, csmDepth->GetTextureID());
	lightingShader->setInt("shadowMap", 3);
	for (GameObject& gameObject : Scene::GetGameObjects()) {
		if (gameObject.GetName() == "Plane0") {
			lightingShader->setMat4("model", gameObject.GetModelMatrix());
			lightingShader->setVec2("textureScale", gameObject.GetTextureScale());

			AssetManager::DrawModel("Plane", *lightingShader);
		}
	}

	refractionFBO->Unbind();
	glViewport(0, 0, Window::m_windowWidth, Window::m_windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::WaterPass() {
	Shader* waterShader = GetShaderByName("Water");
	Camera* camera = CameraManager::GetActiveCamera();
	FrameBuffer* refractionFrameBuffer = GetFrameBufferByName("Refraction");

	std::vector<LightCreateInfo>& sceneLights = GetSceneLights();

	static float moveFactor = 0.0f;
	float waveSpeed = 0.03f;
	moveFactor += waveSpeed * Window::GetDeltaTime();
	if (moveFactor > 1.0f)
		moveFactor -= 1.0f;

	waterShader->activate();
	waterShader->setMat4("view", camera->GetViewMatrix());
	waterShader->setMat4("projection", camera->GetProjectionMatrix());
	waterShader->setVec3("camPos", CameraManager::GetActiveCamera()->cameraPos);

	for (int i = 0; i < sceneLights.size(); i++) {
		std::string lightUniform = "lights[" + std::to_string(i) + "]";

		waterShader->setFloat(lightUniform + ".posX", sceneLights[i].position.x);
		waterShader->setFloat(lightUniform + ".posY", sceneLights[i].position.y);
		waterShader->setFloat(lightUniform + ".posZ", sceneLights[i].position.z);
		waterShader->setFloat(lightUniform + ".colorR", sceneLights[i].color.r);
		waterShader->setFloat(lightUniform + ".colorG", sceneLights[i].color.g);
		waterShader->setFloat(lightUniform + ".colorB", sceneLights[i].color.b);
		waterShader->setFloat(lightUniform + ".radius", sceneLights[i].radius);
	}
	waterShader->setInt("lightsNr", sceneLights.size());

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, refractionFrameBuffer->GetColorAttachmentTextureIdByIndex(0));
	waterShader->setInt("refractionColor", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, refractionFrameBuffer->GetDepthTextureAttachmentId());
	waterShader->setInt("depthTexture", 5);

	waterShader->setFloat("moveFactor", moveFactor);

	for (WaterObject& waterPlaneObject : Scene::GetWaterPlaneObjects()) {
		glm::mat4 lmodel = glm::mat4(1.0f);
		lmodel = glm::translate(lmodel, waterPlaneObject.GetPosition());
		lmodel = glm::scale(lmodel, waterPlaneObject.GetSize());
		lmodel *= waterPlaneObject.GetRotationMatrix();
		waterShader->setMat4("model", lmodel);
		AssetManager::DrawModel("WaterPlane", *waterShader);
	}
}