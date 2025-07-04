#include "./Renderer.h"

void OpenGLRenderer::UIPass() {
	Shader* uiShader = GetShaderByName("UI");
	Mesh2D* textMesh = GetQuadMeshByName("Text");
	Mesh2D* textureMesh = GetQuadMeshByName("Texture");

	glDisable(GL_DEPTH_TEST);
	glm::mat4 UiProjection = glm::ortho(0.0f, (float)Window::m_windowWidth, (float)Window::m_windowHeight, 0.0f);
	Texture* sansFontTexture = AssetManager::GetTextureByName("sans.png");

	uiShader->activate();
	for (UITextElement& uiTextElement : UIManager::GetTextElements()) {
		uiShader->setMat4("projection", UiProjection);
		uiShader->setMat4("model", glm::mat4(1.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sansFontTexture->m_id);

		textMesh->RenderText(uiTextElement.text, uiTextElement.posX, uiTextElement.posY, uiTextElement.size, uiTextElement.fontColor, *uiShader);
	}

	// CROSSHAIR DOT 
	UIElement crosshairElement;
	crosshairElement.m_size = 50.0f;
	crosshairElement.m_useAligment = true;
	crosshairElement.m_alignment = UIAlignment::Center;

	uiShader->activate();
	uiShader->setMat4("projection", UiProjection);
	uiShader->setMat4("model", crosshairElement.GetModelMatrix());
	uiShader->setInt("baseTexture", 0);

	Texture* crosshairTexture = AssetManager::GetTextureByName("CrossHairDotOutline.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, crosshairTexture->m_id);


	textureMesh->RenderTexture(*uiShader);


	glEnable(GL_DEPTH_TEST);
}