#include "./Renderer.h"

namespace OpenGLRenderer {
	GLuint g_finalBlurTexture = 0;

	void BloomPass() {
		FrameBuffer* msaaFBO = GetFrameBufferByName("MSAAPostProcess");
		FrameBuffer* postProcessingFBO = GetFrameBufferByName("PostProcess");

		Mesh2D* postProcessQuad = GetQuadMeshByName("PostProcess");

		Shader* blurHShader = GetShaderByName("BlurHorizontal");
		Shader* blurVShader = GetShaderByName("BlurVertical");

		FrameBuffer* pingFBO = GetFrameBufferByName("BloomPing");
		FrameBuffer* pongFBO = GetFrameBufferByName("BloomPong");

		GLuint pingTex = pingFBO->GetColorAttachmentTextureIdByIndex(0);
		GLuint pongTex = pongFBO->GetColorAttachmentTextureIdByIndex(0);

		bool horizontal = true, firstIteration = true;
		int blurPasses = 4;

		if (!BloomEnabled()) {
			pingFBO->Bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			pongFBO->Bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			g_finalBlurTexture = 0;
		}
		else {
			for (int i = 0; i < blurPasses; ++i) {
				FrameBuffer* currentFBO = horizontal ? pongFBO : pingFBO;
				currentFBO->Bind();
				currentFBO->SetViewport();

				Shader* activeShader = horizontal ? blurHShader : blurVShader;
				activeShader->activate();

				GLuint srcTex = firstIteration ? postProcessingFBO->GetColorAttachmentTextureIdByIndex(1) : (horizontal ? pingTex : pongTex);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, srcTex);
				activeShader->setInt("image", 0);
				activeShader->setFloat("sampleDistance", 3.0f);

				glBindVertexArray(postProcessQuad->GetVAO());
				glDisable(GL_DEPTH_TEST);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				g_finalBlurTexture = currentFBO->GetColorAttachmentTextureIdByIndex(0);

				horizontal = !horizontal;
				if (firstIteration)
					firstIteration = false;
			}
		}
	}

	GLuint GetFinalBlurTexture() {
		return g_finalBlurTexture;
	}
}