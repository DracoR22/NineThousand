#include "./Renderer.h"

namespace OpenGLRenderer {
	void BloodSplatterPass() {
		Camera* camera = CameraManager::GetActiveCamera();

		Model* bloodModel4 = AssetManager::GetModelByName("blood_mesh4");
		Model* bloodModel6 = AssetManager::GetModelByName("blood_mesh6");
		Model* bloodModel7 = AssetManager::GetModelByName("blood_mesh7");
		Model* bloodModel9 = AssetManager::GetModelByName("blood_mesh9");

		Texture* bloodPos4Texture = AssetManager::GetTextureByName("blood_pos4.exr");
		Texture* bloodPos6Texture = AssetManager::GetTextureByName("blood_pos6.exr");
		Texture* bloodPos7Texture = AssetManager::GetTextureByName("blood_pos7.exr");
		Texture* bloodPos9Texture = AssetManager::GetTextureByName("blood_pos9.exr");

		Texture* bloodNorm4Texture = AssetManager::GetTextureByName("blood_pos4.exr");
		Texture* bloodNorm6Texture = AssetManager::GetTextureByName("blood_pos6.exr");
		Texture* bloodNorm7Texture = AssetManager::GetTextureByName("blood_pos7.exr");
		Texture* bloodNorm9Texture = AssetManager::GetTextureByName("blood_pos9.exr");

		std::vector<BloodSplatterObject>& bloodSplatterObjects = Scene::GetBloodSplatterObjects();
		Shader* bloodSplatterShader = GetShaderByName("BloodSplatter");

		bloodSplatterShader->activate();
		bloodSplatterShader->setMat4("view", camera->GetViewMatrix());
		bloodSplatterShader->setMat4("projection", camera->GetProjectionMatrix());
		bloodSplatterShader->setInt("u_PosTex", 0);
		bloodSplatterShader->setInt("u_NormTex", 1);
		for (BloodSplatterObject& bloodObject : bloodSplatterObjects) {
			bloodSplatterShader->setFloat("u_time", bloodObject.GetLifeTime());
			bloodSplatterShader->setMat4("model", bloodObject.GetModelMatrix());
			bloodSplatterShader->setMat4("inverseModel", glm::inverse(bloodObject.GetModelMatrix()));

			Model* model = nullptr;

			if (bloodObject.GetType() == 4) {
				model = bloodModel4;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, bloodPos4Texture->m_id);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, bloodNorm4Texture->m_id);
			}
			else if (bloodObject.GetType() == 6) {
				model = bloodModel6;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, bloodPos6Texture->m_id);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, bloodNorm6Texture->m_id);
			}
			else if (bloodObject.GetType() == 7) {
				model = bloodModel7;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, bloodPos7Texture->m_id);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, bloodNorm7Texture->m_id);
			}
			else if (bloodObject.GetType() == 9) {
				model = bloodModel9;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, bloodPos9Texture->m_id);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, bloodNorm9Texture->m_id);
			}

			if (model) {
				for (Mesh& mesh : model->m_meshes) {
					glBindVertexArray(mesh.GetVAO());
					glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
				}
			}
		}
		glBindVertexArray(0);
	}
}