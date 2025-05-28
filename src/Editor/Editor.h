#pragma once

#include "../Camera/Camera.h"
#include "../Core/Window.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Editor/EditorPanel.h"

namespace Editor {
	void Init();
	void Update();
	void UpdateCamera();

	Camera& GetCamera();
}