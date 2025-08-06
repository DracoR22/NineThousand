#include "./Renderer.h"

void OpenGLRenderer::PostProcessingPass() {
	RendererType rendererType = GetRenderType();
	RendererCommon::PostProcessMode currentMode = GetPostProcessMode();

	FrameBuffer* msaaFBO = GetFrameBufferByName("MSAA");
	FrameBuffer* postProcessingFBO = GetFrameBufferByName("PostProcess");

	Shader* postProcessShader = GetShaderByName("PostProcess");
	Shader* sobelEdgesShader = GetShaderByName("SobelEdges");

	Mesh2D* postProcessQuad = GetQuadMeshByName("PostProcess");


	FrameBuffer* pingFBO = GetFrameBufferByName("BloomPing");
	FrameBuffer* pongFBO = GetFrameBufferByName("BloomPong");

	GLuint pingTex = pingFBO->GetColorAttachmentTextureIdByIndex(0);
	GLuint pongTex = pongFBO->GetColorAttachmentTextureIdByIndex(0);

	bool horizontal = true, firstIteration = true;
	int blurPasses = 10;

	Shader* blurShader = GetShaderByName("GaussianBlur");
	blurShader->activate();

	for (int i = 0; i < blurPasses; ++i) {
		FrameBuffer* currentFBO = horizontal ? pongFBO : pingFBO;
		currentFBO->Bind();

		blurShader->setBool("horizontal", horizontal);

		GLuint srcTex = firstIteration ? postProcessingFBO->GetColorAttachmentTextureIdByIndex(1) : (horizontal ? pingTex : pongTex);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTex);
		blurShader->setInt("image", 0);

		// draw quad
		glBindVertexArray(postProcessQuad->GetVAO());
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		horizontal = !horizontal;
		if (firstIteration)
			firstIteration = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float gamma = GetGammaValue();
	float exposure = GetExposure();

	// copy the depth buffer of msaa fbo to the postprocess fbo
	/*if (rendererType == RendererType::FORWARD) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO->GetFBO());
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFBO->GetFBO());
	if (postProcessingFBO->GetWidth() == GetRenderResolution().x) {
		glBlitFramebuffer(0, 0, GetRenderResolution().x, GetRenderResolution().y, 0, 0, GetRenderResolution().x, GetRenderResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	  }
	}
	*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::m_windowWidth, Window::m_windowHeight);

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
	glBindTexture(GL_TEXTURE_2D, horizontal ? pingTex : pongTex);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}