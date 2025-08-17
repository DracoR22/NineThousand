#include "./Renderer.h"

namespace OpenGLRenderer {
	void DrawShadows(Shader& shader);

	void ShadowPass() {
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
		DrawShadows(*csmDepthShader);
		glCullFace(GL_BACK);
		csmDepth->Unbind();

		glViewport(0, 0, GetRenderResolution().x, GetRenderResolution().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DrawShadows(Shader& shader) {
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