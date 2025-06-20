#pragma once

#include <vector>
#include "./UIElement.h"
#include "../Camera/CameraManager.h"
#include "./Text2D.h"
#include "../Game/Game.h"

struct UITextElement {
	float size = 1.0f;
	float posX = 1.0f;
	float posY = 1.0f;
	glm::vec3 fontColor = glm::vec3(1.0f);
	std::string text = "";
	bool isVisible = false;
};

namespace UIManager {
	void Init();
	void Update();

	std::vector<UITextElement>& GetTextElements();
	UITextElement* GetTextElementByIndex(int index);
}