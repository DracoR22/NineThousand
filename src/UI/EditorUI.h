#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../Core/Window.h"
#include "../Common/RendererCommon.h"
#include "../API/OpenGL/Renderer.h"
#include "../Game/Player.h"
#include "../Game/Scene.h"

namespace EditorUI {
	void Init();
	void Update();
	void Render();
	void Cleanup();
}