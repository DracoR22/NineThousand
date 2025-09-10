#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../Core/Window.h"
#include "../Common/RendererCommon.h"
#include "../API/OpenGL/Renderer/Renderer.h"
#include "../Game/Player.h"
#include "../Game/Scene.h"
#include "../Utils/JSON.h"
#include "./Editor.h"

namespace EditorPanel {
	void Init();
	void Update();
	void Render();
	void Cleanup();
}