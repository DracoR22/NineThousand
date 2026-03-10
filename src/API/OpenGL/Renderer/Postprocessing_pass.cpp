#include "./Renderer.h"

void OpenGLRenderer::PostProcessingPass() {
	RendererType rendererType = GetRenderType();
	RendererCommon::PostProcessMode currentMode = GetPostProcessMode();

	FrameBuffer* sceneFBO = GetFrameBufferByName("Scene");
	FrameBuffer* compositeSceneFBO = GetFrameBufferByName("CompositeScene");
	FrameBuffer* FXAAFBO = GetFrameBufferByName("FXAA");

	Shader* postProcessShader = GetShaderByName("PostProcess");
	Shader* sobelEdgesShader = GetShaderByName("SobelEdges");
	Shader* FXAAShader = GetShaderByName("FXAA");

	Mesh2D* postProcessQuad = GetQuadMeshByName("PostProcess");

	float gamma = GetGammaValue();
	float exposure = GetExposure();

	glBindFramebuffer(GL_FRAMEBUFFER, compositeSceneFBO->GetFBO());
	glViewport(0, 0, compositeSceneFBO->GetWidth(), compositeSceneFBO->GetHeight());

	switch (currentMode) {
	case RendererCommon::PostProcessMode::NONE:
		postProcessShader->activate();
		postProcessShader->setInt("screenTexture", 0);
		postProcessShader->setInt("bloomTexture", 1);
		postProcessShader->setFloat("gamma", gamma);
		postProcessShader->setFloat("exposure", exposure);

		break;
	case RendererCommon::PostProcessMode::SHARPEN:
		sobelEdgesShader->activate();
		sobelEdgesShader->setInt("screenTexture", 0);
		break;
	default:
		postProcessShader->activate();
		postProcessShader->setInt("screenTexture", 0);
		postProcessShader->setInt("bloomTexture", 1);
		break;
	}

	// draw quad
	glBindVertexArray(postProcessQuad->GetVAO());
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneFBO->GetColorAttachmentTextureIdByIndex(0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GetFinalBlurTexture());

	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	// FXAA
	FXAAFBO->Bind();
	FXAAFBO->SetViewport();

	FXAAShader->activate();
	FXAAShader->setInt("screenTexture", 0);
	FXAAShader->setVec2("resolution", glm::vec2(FXAAFBO->GetWidth(), FXAAFBO->GetHeight()));

	glBindVertexArray(postProcessQuad->GetVAO());
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compositeSceneFBO->GetColorAttachmentTextureIdByIndex(0));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// final pass (copy stuff from final image fbo to the main fbo)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FXAAFBO->GetFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, FXAAFBO->GetWidth(), FXAAFBO->GetHeight(),
		0, 0, Window::m_windowWidth, Window::m_windowHeight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);
}