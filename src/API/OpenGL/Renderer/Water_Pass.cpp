#include "./Renderer.h"

namespace OpenGLRenderer {
	void DrawWaterObjects(Shader& waterShader);

	void PreWaterPass() {
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
				lightingShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(gameObject.GetModelMatrix()))));

				Model* gameObjectModel = AssetManager::GetModelByName(gameObject.GetModelName());
				for (Mesh& mesh : gameObjectModel->m_meshes) {
					int materialIndex = gameObject.GetMeshMaterialIndex(mesh.m_Name);
					Material* meshMaterial = AssetManager::GetMaterialByIndex(materialIndex);
					Texture* baseTexture = AssetManager::GetTextureByIndex(meshMaterial->baseTexture);
					Texture* normalTexture = AssetManager::GetTextureByIndex(meshMaterial->normalTexture);
					Texture* rmaTexture = AssetManager::GetTextureByIndex(meshMaterial->rmaTexture);

					glActiveTexture(GL_TEXTURE0);
					lightingShader->setInt("baseTexture", 0);
					baseTexture->Bind();

					glActiveTexture(GL_TEXTURE1);
					lightingShader->setInt("normalTexture", 1);
					normalTexture->Bind();

					glActiveTexture(GL_TEXTURE2);
					lightingShader->setInt("rmaTexture", 2);
					rmaTexture->Bind();

					glBindVertexArray(mesh.GetVAO());
					glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}
		}

		refractionFBO->Unbind();
		glViewport(0, 0, Window::m_windowWidth, Window::m_windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void WaterPass() {
		Shader* waterShader = GetShaderByName("Water");
		Camera* camera = CameraManager::GetActiveCamera();
		FrameBuffer* refractionFrameBuffer = GetFrameBufferByName("Refraction");

		Texture* distortionTexture = AssetManager::GetTextureByName("WaterDUDV.png");
		Texture* normalMapTexture = AssetManager::GetTextureByName("WaterNormal.png");

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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, refractionFrameBuffer->GetColorAttachmentTextureIdByIndex(0));
		waterShader->setInt("refractionColor", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionFrameBuffer->GetDepthTextureAttachmentId());
		waterShader->setInt("depthTexture", 1);

		glActiveTexture(GL_TEXTURE2);
		distortionTexture->Bind();
		waterShader->setInt("distortionTexture", 2);

		glActiveTexture(GL_TEXTURE3);
		normalMapTexture->Bind();
		waterShader->setInt("normalTexture", 3);

		waterShader->setFloat("moveFactor", moveFactor);
		DrawWaterObjects(*waterShader);
	}

	void DrawWaterObjects(Shader& waterShader) {
		for (WaterObject& waterPlaneObject : Scene::GetWaterPlaneObjects()) {
			glm::mat4 lmodel = glm::mat4(1.0f);
			lmodel = glm::translate(lmodel, waterPlaneObject.GetPosition());
			lmodel = glm::scale(lmodel, waterPlaneObject.GetSize());
			lmodel *= waterPlaneObject.GetRotationMatrix();
			waterShader.setMat4("model", lmodel);
			Model* waterObjectModel = AssetManager::GetModelByName("Plane");

			for (Mesh& mesh : waterObjectModel->m_meshes) {
				glBindVertexArray(mesh.GetVAO());
				glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
	}
}