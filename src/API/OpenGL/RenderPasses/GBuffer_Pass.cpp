#include "../Renderer.h"

void OpenGLRenderer::GBufferPass() {
	Shader* gBufferShader = GetShaderByName("GBuffer");
	FrameBuffer* gBuffer = GetFrameBufferByName("GBuffer");

	if (gBufferShader == nullptr) {
		std::cerr << "OpenGLRenderer::GBufferPass() error: shader is nullptr" << std::endl;
		return;
	}

	if (gBuffer == nullptr) {
		std::cerr << "OpenGLRenderer::GBufferPass() error: frameBuffer is nullptr" << std::endl;
		return;
	}

	Camera* camera = CameraManager::GetActiveCamera();
	glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	Frustum& frustum = camera->GetFrustum();

	gBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	gBufferShader->activate();
	gBufferShader->setMat4("projection", projectionMatrix);
	gBufferShader->setMat4("view", viewMatrix);
	for (GameObject& gameObject : Scene::GetGameObjects()) {
		gBufferShader->setMat4("model", gameObject.GetModelMatrix());
		//g_shaders.gBufferShader.setVec2("textureScale", gameObject.GetTextureScale());

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

		if (frustum.IntersectsAABB(worldAABB)) {
			AssetManager::DrawModel(gameObject.GetModelName(), *gBufferShader);
		}

	}
	glStencilMask(0x00);
	gBuffer->Unbind();
}