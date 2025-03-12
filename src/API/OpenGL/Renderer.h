#pragma once


#include "Shader.h"
#include "Cubemap.h"
#include "ShadowMap.h"

#include "./FrameBuffer.hpp"

#include "../../Input/Camera.h"
#include "../../Input/Keyboard.h"
#include "../../Input/Mouse.h"

#include "../../Physics/Physics.h"

#include "../../Animations/Animation.hpp"
#include "../../Animations/Animator.hpp"

#include "../../Game/Player.h"
#include "../../Game/Game.h"

#include "../../Core/Window.h"
#include "../../Core/AssetManager.h"


namespace OpenGLRenderer {
	inline float g_deltaTime = 0.0f;
	inline float g_lastFrame = 0.0f;

	inline int g_fps = 0;

	inline float g_gamma = 2.2f;

	inline std::vector<PointLight> g_sceneLights = {};


	void Init();
	void RenderFrame();
}