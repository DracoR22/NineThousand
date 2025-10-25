#include "./Renderer.h"

namespace OpenGLRenderer {
	void PickUpPass() {
		Shader* shader = GetShaderByName("PickUp");
		Camera* camera = CameraManager::GetActiveCamera();

		Model* pickUpModel = AssetManager::GetModelByName("AKS74U_Pickup");

		static float elapsedTime = 0.0f;
		elapsedTime += Window::GetDeltaTime();
		if (elapsedTime > 10000.0f) elapsedTime = 0.0f;

		shader->activate();
		shader->setMat4("viewMatrix", camera->GetViewMatrix());
		shader->setMat4("projectionMatrix", camera->GetProjectionMatrix());
		shader->setInt("numLights", Scene::GetLightObjects().size());
		shader->set3Float("viewPos", camera->cameraPos);
		shader->setFloat("time", elapsedTime);

		for (PickUpObject& object : Scene::GetPickUpObjects()) {
			if (!object.IsCollected()) {
				shader->setMat4("modelMatrix", object.GetModelMatrix());
				shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(object.GetModelMatrix()))));
				for (Mesh& mesh : pickUpModel->m_meshes) {
					int materialIndex = object.GetMeshMaterialIndex(mesh.m_Name);
					Material* meshMaterial = AssetManager::GetMaterialByIndex(materialIndex);

					Texture* baseTexture = AssetManager::GetTextureByIndex(meshMaterial->baseTexture);
					Texture* normalTexture = AssetManager::GetTextureByIndex(meshMaterial->normalTexture);
					Texture* rmaTexture = AssetManager::GetTextureByIndex(meshMaterial->rmaTexture);

					glActiveTexture(GL_TEXTURE0);
					baseTexture->Bind();
					shader->setInt("baseTexture", 0);

					glActiveTexture(GL_TEXTURE1);
					normalTexture->Bind();
					shader->setInt("normalTexture", 1);

					glActiveTexture(GL_TEXTURE2);
					rmaTexture->Bind();
					shader->setInt("rmaTexture", 2);

					glBindVertexArray(mesh.GetVAO());
					glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}
		}
	}
}