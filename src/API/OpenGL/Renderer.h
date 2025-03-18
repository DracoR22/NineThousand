#pragma once


#include "Shader.h"
#include "Cubemap.h"
#include "ShadowMap.h"

#include "./FrameBuffer.h"

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

#include "../../Common/RendererCommon.h"


namespace OpenGLRenderer {
	void Init();
	void RenderFrame();

	RendererCommon::PostProcessMode GetPostProcessMode();
	void ChangePostProcessMode(RendererCommon::PostProcessMode mode);

	float GetGammaValue();
	void ChangeGammaValue(float value);

	void Cleanup();
}