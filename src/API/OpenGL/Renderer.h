#pragma once

#include "Shader.h"
#include "Cubemap.h"
#include "ShadowMap.h"
#include "Mesh2D.hpp"

#include "./FrameBuffer.h"

#include "../../Camera/Camera.h"
#include "../../Camera/CameraManager.h"
#include "../../Camera/Frustum.h"

#include "../../Input/Keyboard.h"
#include "../../Input/Mouse.h"

#include "../../Physics/Physics.h"

#include "../../Animation/Animation.hpp"
#include "../../Animation/Animator.hpp"

#include "../../Game/Player.h"
#include "../../Game/Game.h"
#include "../../Game/Scene.h"

#include "../../Core/Window.h"
#include "../../Core/AssetManager.h"

#include "../../Common/RendererCommon.h"
#include "../../Common/AABB.h"

#include "../../UI/UIElement.h"

#include "../../Utils/Utils.h"

#define GLCheckError() \
{ \
    GLenum err; \
    while ((err = glGetError()) != GL_NO_ERROR) \
        std::cerr << "OpenGL Error: " << err << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
}

namespace OpenGLRenderer {
	void Init();
	void RenderFrame();

	// render passes
	void ShadowPass(); // TODO!
	void WaterRefractionPass(); // TODO!

	// stuff for the editor
	RendererCommon::PostProcessMode GetPostProcessMode();
	void ChangePostProcessMode(RendererCommon::PostProcessMode mode);

	float GetExposure();
	void SetExposureValue(float value);
	float GetGammaValue();
	void ChangeGammaValue(float value);

	std::vector<LightCreateInfo>& GetSceneLights();
	void UpdateLightStrength(int index, float strength);
	void UpdateLightRadius(int index, float radius);
	void UpdateLightPosition(int index, glm::vec3 newPosition);

	void Cleanup();
}