#include "./Renderer.h"

void OpenGLRenderer::PostProcessingPass() {
	RendererType rendererType = GetRenderType();
	RendererCommon::PostProcessMode currentMode = GetPostProcessMode();

	FrameBuffer* msaaFBO = GetFrameBufferByName("MSAAPostProcess");
	FrameBuffer* postProcessingFBO = GetFrameBufferByName("PostProcess");
	FrameBuffer* finalImageFBO = GetFrameBufferByName("FinalImage");

	Shader* postProcessShader = GetShaderByName("PostProcess");
	Shader* sobelEdgesShader = GetShaderByName("SobelEdges");

	Mesh2D* postProcessQuad = GetQuadMeshByName("PostProcess");

	float gamma = GetGammaValue();
	float exposure = GetExposure();

	// copy the depth buffer of msaa fbo to the postprocess fbo
	/*if (rendererType == RendererType::FORWARD) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO->GetFBO());
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO->GetFBO());
	if (postProcessingFBO->GetWidth() == GetRenderResolution().x) {
		glBlitFramebuffer(0, 0, GetRenderResolution().x, GetRenderResolution().y, 0, 0, GetRenderResolution().x, GetRenderResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	 }*/


	//FrameBuffer* msaaFBO = GetFrameBufferByName("MSAAPostProcess");
	//FrameBuffer* postProcessingFBO = GetFrameBufferByName("PostProcess");
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO->GetFBO());
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO->GetFBO());

	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glBlitFramebuffer(0, 0, GetRenderResolution().x, GetRenderResolution().y, 0, 0, GetRenderResolution().x, GetRenderResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);


	//glReadBuffer(GL_COLOR_ATTACHMENT1);
	//glDrawBuffer(GL_COLOR_ATTACHMENT1);
	//glBlitFramebuffer(0, 0, GetRenderResolution().x, GetRenderResolution().y, 0, 0, GetRenderResolution().x, GetRenderResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//// Restore original state.
	//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	//glDrawBuffers(2, attachments);

	glm::vec2 viewPortPos = GetViewportPos();
	glm::vec2 viewPortSize = GetViewportSize();

	glBindFramebuffer(GL_FRAMEBUFFER, finalImageFBO->GetFBO());
	glViewport(0, 0, finalImageFBO->GetWidth(), finalImageFBO->GetHeight());

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
	glBindTexture(GL_TEXTURE_2D, postProcessingFBO->GetColorAttachmentTextureIdByIndex(0));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GetFinalBlurTexture());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// final pass (copy stuff from final image fbo to the main fbo)
	glBindFramebuffer(GL_READ_FRAMEBUFFER, finalImageFBO->GetFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0, 0, finalImageFBO->GetWidth(), finalImageFBO->GetHeight(),
		0, 0, Window::m_windowWidth, Window::m_windowHeight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
	);
}