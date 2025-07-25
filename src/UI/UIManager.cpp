#include "./UIManager.h"

namespace UIManager {
	std::vector<UITextElement> g_uiTextElements;
	std::vector<UITextureElement> g_uiTextureElements;

	void Init() {
		// text elements
		g_uiTextElements.clear();

		UITextElement& fpsText = g_uiTextElements.emplace_back();
		fpsText.size = 0.22f;
		fpsText.posX = 10.0f;
		fpsText.posY = 30.0f;
		fpsText.isVisible = true;
		fpsText.text = "FPS: 0";

		UITextElement& playerPositionText = g_uiTextElements.emplace_back();
		playerPositionText.size = 0.22f;
		playerPositionText.posX = 10.0f;
		playerPositionText.posY = 55.0f;
		playerPositionText.isVisible = true;
		playerPositionText.text = "Player Position: (0, 0, 0)";

		UITextElement& magAmmoText = g_uiTextElements.emplace_back();
		magAmmoText.size = 0.50f;
		magAmmoText.posX = 30.0f;
		magAmmoText.posY = 50.0f;
		magAmmoText.isVisible = true;
		magAmmoText.text = "000";

		UITextElement& reserveAmmoText = g_uiTextElements.emplace_back();
		reserveAmmoText.size = 0.25f;
		reserveAmmoText.posX = 30.0f;
		reserveAmmoText.posY = 50.0f;
		reserveAmmoText.isVisible = true;
		reserveAmmoText.text = "/ 999";

		// texture elements
		g_uiTextureElements.clear();

		UITextureElement& crosshairDot = g_uiTextureElements.emplace_back();
		crosshairDot.m_size = 50.0f;
		crosshairDot.m_alignment = UIAlignment::Center;
		crosshairDot.m_isVisible = false;
		crosshairDot.m_textureName = "CrossHairDotOutline.png";

		UITextureElement& crossHairArrow = g_uiTextureElements.emplace_back();
		crossHairArrow.m_size = 50.0f;
		crossHairArrow.m_alignment = UIAlignment::Center;
		crossHairArrow.m_isVisible = false;
		crossHairArrow.m_textureName = "CrossHairArrow.png";
	}

	void Update() {
		Player& player = Game::GetPLayerByIndex(0);

		if (g_uiTextElements.size() >= 4) {
			WeaponState* weaponState = player.GetEquipedWeaponState();

			int fps = Window::GetFPSCount();
			std::string playerPosition = "Player Position: (" +
				std::to_string(CameraManager::GetActiveCamera()->cameraPos.x) + ", " +
				std::to_string(CameraManager::GetActiveCamera()->cameraPos.y) + ", " +
				std::to_string(CameraManager::GetActiveCamera()->cameraPos.z) + ")";

			std::ostringstream currentAmmoOss;
			currentAmmoOss << std::setw(3) << std::setfill('0') << weaponState->ammoInMag;

			g_uiTextElements[0].text = "FPS: " + std::to_string(fps);
			g_uiTextElements[1].text = playerPosition;
			g_uiTextElements[2].text = currentAmmoOss.str();
			float textWidth = Text2D::GetTextWidth(g_uiTextElements[2].text, g_uiTextElements[2].size);
			float textHeight = Text2D::GetTextHeight(g_uiTextElements[2].size);
			g_uiTextElements[2].posX = Window::m_windowWidth - textWidth - 103.0f;
			g_uiTextElements[2].posY = Window::m_windowHeight - textHeight - 70.0f;

			g_uiTextElements[3].posX = Window::m_windowWidth - textWidth - 30.0f;
			g_uiTextElements[3].posY = Window::m_windowHeight - textHeight - 50.0f;
		}

		if (!g_uiTextureElements.empty()) {
			// change crosshair to arrow when player is sprinting
			if (player.IsSprinting()) {
				g_uiTextureElements[0].m_isVisible = false;
				g_uiTextureElements[1].m_isVisible = true;
			}
			else {
				g_uiTextureElements[0].m_isVisible = true;
				g_uiTextureElements[1].m_isVisible = false;
			}

			// hide crosshair when in ADS
			if (player.IsInADS()) {
				g_uiTextureElements[0].m_isVisible = false;
				g_uiTextureElements[1].m_isVisible = false;
			}
		}
	}

	std::vector<UITextureElement>& GetTextureElements() {
		return g_uiTextureElements;
	}

	std::vector<UITextElement>& GetTextElements() {
		return g_uiTextElements;
	}

	UITextElement* GetTextElementByIndex(int index) {
		if (index < 0 || index >= static_cast<int>(g_uiTextElements.size())) {
			std::cout << "UIManager::GetTextElementByIndex() failed because text element at index" << index << "does not exist!\n";
			return nullptr;
		}

		return &g_uiTextElements[index];
	}
}