#include "./Renderer.h"

namespace OpenGLRenderer {
	void DrawShadows(Shader& shader);

	void ShadowPass() {
		ShadowMap* csmDepth = GetShadowMapByName("CSM");
		Shader* csmDepthShader = GetShaderByName("CSM");
		
		SSBO* lightsSpaceMatricesSSBO = GetSSBOByName("LightSpaceMatricesCSM");

		if (csmDepthShader == nullptr) {
			std::cerr << "OpenGLRenderer::ShadowPass() error: shader is nullptr" << std::endl;
			return;
		}

		Camera* camera = CameraManager::GetActiveCamera();
		std::vector<float>& shadowCascadeLevels = GetShadowCascadeLevels();

		const auto lightMatrices = Utils::GetLightSpaceMatrices(camera->GetNearPlane(), camera->GetFarPlane(), shadowCascadeLevels, (float)Window::m_windowWidth, (float)Window::m_windowHeight, camera->getZoom(), camera->GetViewMatrix());
		lightsSpaceMatricesSSBO->Update(sizeof(glm::mat4) * lightMatrices.size(), lightMatrices.data());
		lightsSpaceMatricesSSBO->Bind(1);

		csmDepth->Clear();
		glCullFace(GL_FRONT);
		csmDepthShader->activate();
		DrawShadows(*csmDepthShader);
		glCullFace(GL_BACK);
	}

	void DrawShadows(Shader& shader) {
		if (!ShadowsEnabled()) return;

		for (GameObject& gameObject : Scene::GetGameObjects()) {
			if (gameObject.GetModelName() == "Cube" || gameObject.GetModelName() == "Barrel") {
				Model* gameObjectModel = AssetManager::GetModelByName(gameObject.GetModelName());

				shader.setMat4("model", gameObject.GetModelMatrix());

				for (Mesh& mesh : gameObjectModel->m_meshes) {
					glBindVertexArray(mesh.GetVAO());
					glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}
		}
	}
}