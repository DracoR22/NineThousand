#pragma once

#include "../Camera/Camera.h"
#include "../Core/Window.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Editor/EditorPanel.h"

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

namespace Editor {
	void Init();
	void Update();
	void UpdateCamera();

	glm::vec3 GetMouseRayOrigin();

	Camera& GetCamera();
}