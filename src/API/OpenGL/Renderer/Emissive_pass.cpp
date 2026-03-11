#include "./Renderer.h"

namespace OpenGLRenderer {
	GLuint g_finalBlurTexture = 0;

	void EmissivePass() {
		FrameBuffer* sceneFBO = GetFrameBufferByName("Scene");

		Mesh2D* quadMesh = GetQuadMeshByName("Quad");

		Shader* blurHShader = GetShaderByName("BlurHorizontal");
		Shader* blurVShader = GetShaderByName("BlurVertical");

		FrameBuffer* emissiveFBO = GetFrameBufferByName("Emissive");
		
		GLuint pingTex = emissiveFBO->GetColorAttachmentTextureIdByName("ColorA");
		GLuint pongTex = emissiveFBO->GetColorAttachmentTextureIdByName("ColorB");

		bool horizontal = true, firstIteration = true;
		int blurPasses = 4;

		if (!BloomEnabled()) {
			emissiveFBO->Bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			g_finalBlurTexture = 0;
		}
		else {
			emissiveFBO->Bind();
			emissiveFBO->SetViewport();

			for (int i = 0; i < blurPasses; ++i) {
				const char* dstAttachment = horizontal ? "ColorB" : "ColorA";
				GLuint dstTex = horizontal ? pongTex : pingTex;
				GLuint srcTex = firstIteration ? sceneFBO->GetColorAttachmentTextureIdByIndex(1) : (horizontal ? pingTex : pongTex);

				emissiveFBO->DrawBuffer(dstAttachment);

				Shader* activeShader = horizontal ? blurHShader : blurVShader;
				activeShader->activate();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, srcTex);
				activeShader->setInt("image", 0);
				activeShader->setFloat("sampleDistance", 3.0f);

				glBindVertexArray(quadMesh->GetVAO());
				glDisable(GL_DEPTH_TEST);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				g_finalBlurTexture = dstTex;

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