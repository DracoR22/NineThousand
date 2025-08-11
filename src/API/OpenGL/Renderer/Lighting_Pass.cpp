#include "./Renderer.h"

namespace OpenGLRenderer {
	void DrawModel();

	void LightingPass() {
		if (GetRenderType() == RendererType::DEFERRED) {
			DeferredLightingPass();
		}
		else if (GetRenderType() == RendererType::FORWARD) {
			ForwardLightingPass();
		}
	}

	void ForwardLightingPass() {
		Shader* lightingShader = GetShaderByName("Lighting");
		Shader* bloomShader = GetShaderByName("Bloom");

		ShadowMap* csmDepth = GetShadowMapByName("CSM");

		Frustum camFrustum = CameraManager::GetActiveCamera()->GetFrustum();
		Camera* camera = CameraManager::GetActiveCamera();

		std::vector<LightCreateInfo>& sceneLights = GetSceneLights();
		std::vector<float>& shadowCascadeLevels = GetShadowCascadeLevels();

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
		lightingShader->set3Float("camPos", camera->cameraPos);
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
			lightingShader->setMat4("model", gameObject.GetModelMatrix());
			lightingShader->setVec2("textureScale", gameObject.GetTextureScale());
			lightingShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(gameObject.GetModelMatrix()))));

			if (gameObject.IsSelected()) {
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
			}
			else {
				glStencilMask(0x00);
			}

			Model* model = AssetManager::GetModelByName(gameObject.GetModelName());
			AABB modelAABB = AABB(model->GetAABBMin(), model->GetAABBMax());

			glm::mat4 modelMatrix = gameObject.GetModelMatrix();
			AABB worldAABB = modelAABB.TransformModelToWorldMatrix(modelMatrix);

			if (camFrustum.IntersectsAABB(worldAABB)) {
				//model->draw(*lightingShader);

				for (Mesh& mesh : model->m_meshes) {
					// TODO: dont hardcode!!
					if (mesh.m_Name == "Lamp_Outer_Glass" || mesh.m_Name == "Lamp_Inner_Glass") continue;

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

			// draw bloomed stuff
			for (Mesh& mesh : model->m_meshes) {
				if (mesh.m_Name == "Lamp_Outer_Glass") {
					bloomShader->activate();
					bloomShader->setMat4("model", gameObject.GetModelMatrix());
					bloomShader->setMat4("view", camera->GetViewMatrix());
					bloomShader->setMat4("projection", camera->GetProjectionMatrix());
					bloomShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
					glBindVertexArray(mesh.GetVAO());
					glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
					lightingShader->activate();
				}
			}
		}

		Model* stalkerModel = AssetManager::GetModelByName("Stalker");
		for (Stalker& stalkerObject : Game::GetAllStalkers()) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, stalkerObject.GetPosition());

			glm::vec3 rotationRadians = glm::radians(stalkerObject.GetRotationEuler());
			glm::mat4 rotationMatrix = glm::toMat4(glm::quat(rotationRadians));
			model *= rotationMatrix;
			model = glm::scale(model, glm::vec3(5.0f));
			lightingShader->setMat4("model", model);
			lightingShader->setVec2("textureScale", glm::vec2(1.0f));
			lightingShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			for (Mesh& mesh : stalkerModel->m_meshes) {
				Material* meshMaterial = AssetManager::GetMaterialByName("Stalker");
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

		glStencilMask(0x00);
	}

	void DeferredLightingPass() {
		unsigned int lightQuadVAO = 0;
		unsigned int lightQuadVBO = 0;

		Shader* deferredLightingShader = GetShaderByName("DeferredLighting");

		FrameBuffer* gBuffer = GetFrameBufferByName("GBuffer");
		FrameBuffer* postProcessingFrameBuffer = GetFrameBufferByName("PostProcess");

		std::vector<LightCreateInfo>& sceneLights = GetSceneLights();

		postProcessingFrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		deferredLightingShader->activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentTextureIdByIndex(0));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentTextureIdByIndex(1));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentTextureIdByIndex(2));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gBuffer->GetColorAttachmentTextureIdByIndex(3));
		deferredLightingShader->setInt("gPosition", 0);
		deferredLightingShader->setInt("baseTexture", 1);
		deferredLightingShader->setInt("normalTexture", 2);
		deferredLightingShader->setInt("rmaTexture", 3);
		for (int i = 0; i < sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			deferredLightingShader->setFloat(lightUniform + ".posX", sceneLights[i].position.x);
			deferredLightingShader->setFloat(lightUniform + ".posY", sceneLights[i].position.y);
			deferredLightingShader->setFloat(lightUniform + ".posZ", sceneLights[i].position.z);
			deferredLightingShader->setFloat(lightUniform + ".radius", sceneLights[i].radius);
			deferredLightingShader->setFloat(lightUniform + ".strength", sceneLights[i].strength);
			deferredLightingShader->setFloat(lightUniform + ".colorR", sceneLights[i].color.r);
			deferredLightingShader->setFloat(lightUniform + ".colorG", sceneLights[i].color.g);
			deferredLightingShader->setFloat(lightUniform + ".colorB", sceneLights[i].color.b);
			deferredLightingShader->setInt(lightUniform + ".type", static_cast<int>(sceneLights[i].type));
		}
		deferredLightingShader->setInt("noLights", sceneLights.size());
		deferredLightingShader->set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);
		deferredLightingShader->setInt("shadowMap", 3);
		if (lightQuadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &lightQuadVAO);
			glGenBuffers(1, &lightQuadVBO);
			glBindVertexArray(lightQuadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, lightQuadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(lightQuadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFrameBuffer->GetFBO());
		glBlitFramebuffer(
			0, 0, Window::SCR_WIDTH, Window::SCR_HEIGHT,
			0, 0, Window::SCR_WIDTH, Window::SCR_HEIGHT,
			GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFrameBuffer->GetFBO());
	}
}

