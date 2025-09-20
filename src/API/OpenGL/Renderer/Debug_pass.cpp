#include "./Renderer.h"

void OpenGLRenderer::DebugPass() {
	Shader* outlineAnimatedShader = GetShaderByName("OutlineAnimated");
	Shader* solidColorShader = GetShaderByName("SolidColor");
	Shader* bloomShader = GetShaderByName("Bloom");

	Camera* camera = CameraManager::GetActiveCamera();

	// OUTLINES
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);

	outlineAnimatedShader->activate();
	outlineAnimatedShader->setMat4("view", camera->GetViewMatrix());
	outlineAnimatedShader->setMat4("projection", camera->GetProjectionMatrix());
	outlineAnimatedShader->setFloat("outlineThickness", 0.02f);

	static float accumulatedTime = 0.0f;
	accumulatedTime += Window::GetDeltaTime();
	outlineAnimatedShader->setFloat("time", accumulatedTime);

	for (GameObject& outlineObject : Scene::GetGameObjects()) {
		if (!outlineObject.IsSelected()) {
			continue;
		}

		float baseScale = glm::length(outlineObject.GetSize()) / sqrt(3.0f);
		float thicknessMultiplier = glm::clamp(2.0f / baseScale, 1.0f, 1.1f);

		float outlineScale = 1.01f * thicknessMultiplier;

		glm::mat4 stModel = glm::mat4(1.0f);

		stModel = glm::translate(stModel, outlineObject.GetPosition());

		stModel = glm::scale(stModel, outlineObject.GetSize());
		stModel *= outlineObject.GetRotationMatrix();

		outlineAnimatedShader->setMat4("model", stModel);

		Model* outlineObjectModel = AssetManager::GetModelByName(outlineObject.GetModelName());
		for (Mesh& mesh : outlineObjectModel->m_meshes) {
			glBindVertexArray(mesh.GetVAO());
			glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}


	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glEnable(GL_DEPTH_TEST);

	// Debug Physics
	static bool drawCollisionBoxes = false;

	if (Keyboard::KeyJustPressed(GLFW_KEY_F3)) {
		drawCollisionBoxes = !drawCollisionBoxes;
	}

	if (drawCollisionBoxes) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_DEPTH_TEST);

		solidColorShader->activate();
		solidColorShader->set3Float("viewPos", CameraManager::GetActiveCamera()->cameraPos);
		solidColorShader->setMat4("view", camera->GetViewMatrix());
		solidColorShader->setMat4("projection", camera->GetProjectionMatrix());
		solidColorShader->setVec3("lightColor", 0.0f, 1.0f, 0.9f);

		for (GameObject& gameObject : Scene::GetGameObjects()) {
			if (gameObject.GetPhysicsId() != 0) {
				RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(gameObject.GetPhysicsId());
				RigidStatic* rigidStatic = Physics::GetRigidStaticById(gameObject.GetPhysicsId());

				if (rigidStatic) {
					PxBounds3 bounds = rigidStatic->GetPxRigidStatic()->getWorldBounds();
					PxVec3 center = (bounds.minimum + bounds.maximum) * 0.5f;
					PxVec3 extents = (bounds.maximum - bounds.minimum) * 0.5f;

					glm::vec3 glmCenter = Physics::PxVec3toGlmVec3(center);
					glm::vec3 glmExtents = Physics::PxVec3toGlmVec3(extents);

					solidColorShader->setMat4("model", gameObject.GetModelMatrix());

					Model* debugModel = AssetManager::GetModelByName(gameObject.GetModelName());

					for (unsigned int i = 0; i < debugModel->m_meshes.size(); i++) {
						glBindVertexArray(debugModel->m_meshes[i].GetVAO());
						glDrawElements(GL_TRIANGLES, debugModel->m_meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0);
					}
				}

				if (rigidDynamic) {
					PxBounds3 bounds = rigidDynamic->GetPxRigidDynamic()->getWorldBounds();

					PxVec3 center = (bounds.minimum + bounds.maximum) * 0.5f;
					PxVec3 extents = (bounds.maximum - bounds.minimum) * 0.5f;

					glm::vec3 glmCenter = Physics::PxVec3toGlmVec3(center);
					glm::vec3 glmExtents = Physics::PxVec3toGlmVec3(extents);

					PxTransform physxTransform = rigidDynamic->GetPxRigidDynamic()->getGlobalPose();
					glm::vec3 position = Physics::PxVec3toGlmVec3(physxTransform.p);
					glm::quat rotation = Physics::PxQuatToGlmQuat(physxTransform.q);

					glm::mat4 fixOrientation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0));
					glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
					modelMatrix *= glm::mat4_cast(rotation);
					modelMatrix *= fixOrientation;
					modelMatrix = glm::scale(modelMatrix, glmExtents * 2.0f); // Multiply by 2 to get full size

					solidColorShader->setMat4("model", modelMatrix);

					Model* debugModel = AssetManager::GetModelByName(gameObject.GetModelName());

					for (unsigned int i = 0; i < debugModel->m_meshes.size(); i++) {
						glBindVertexArray(debugModel->m_meshes[i].GetVAO());
						glDrawElements(GL_TRIANGLES, debugModel->m_meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0);
					}
				}
			}
		}

		for (auto& [key, value] : Physics::GetCharacterControllers()) {
			PxController* controller = value.GetPxController();
			PxCapsuleController* capsule = static_cast<physx::PxCapsuleController*>(controller);

			float radius = capsule->getRadius();
			float height = capsule->getHeight();

			physx::PxTransform physxTransform = controller->getActor()->getGlobalPose();
			glm::vec3 position = Physics::PxVec3toGlmVec3(physxTransform.p);
			glm::quat rotation = Physics::PxQuatToGlmQuat(physxTransform.q);

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
			modelMatrix *= glm::mat4_cast(rotation);
			// rotate 90 degrees around X so Y points up correctly for the cube mesh
			glm::mat4 fixOrientation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
			modelMatrix *= fixOrientation;
			modelMatrix = glm::scale(modelMatrix, glm::vec3(radius * 2.0f, height, radius * 2.0f));

			solidColorShader->setMat4("model", modelMatrix);

			Model* debugModel = AssetManager::GetModelByName("Cube");

			for (unsigned int i = 0; i < debugModel->m_meshes.size(); i++) {
				glBindVertexArray(debugModel->m_meshes[i].GetVAO());
				glDrawElements(GL_TRIANGLES, debugModel->m_meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0);
			}
		}

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}