#pragma once

#include "../Shader.h"
#include "../Cubemap.h"
#include "../ShadowMap.h"
#include "../Mesh2D.hpp"
#include "../FrameBuffer.h"
#include "../UBO.hpp"
#include "../SSBO.hpp"

#include "../../../Camera/Camera.h"
#include "../../../Camera/CameraManager.h"

#include "../../../Input/Keyboard.h"
#include "../../../Input/Mouse.h"

#include "../../../Physics/Physics.h"

#include "../../../Animation/Animation.hpp"
#include "../../../Animation/Animator.hpp"

#include "../../../Game/Player.h"
#include "../../../Game/Game.h"
#include "../../../Game/Scene.h"

#include "../../../Core/Window.h"
#include "../../../Core/AssetManager.h"

#include "../../../Common/RendererCommon.h"
#include "../../../Common/AABB.h"

#include "../../../Editor/EditorPanel.h"

#include "../../../UI/UITextureElement.h"
#include "../../../UI/UIManager.h"

#include "../../../Utils/Utils.h"

enum class RendererType {
	FORWARD,
	DEFERRED
};

namespace OpenGLRenderer {
	void Init();
	void Render();

	void GBufferPass();
	void ShadowPass();
	void RefractionPass();
	void WaterPass();
	void AnimationPass();
	void LightingPass();
	void ForwardLightingPass();
	void DeferredLightingPass();
	void DebugPass();
	void OutlinePass();
	void BloodSplatterPass();
	void BillboardPass();
	void UIPass();
	void BloomPass();
	void PostProcessingPass();
	void CubeMapPass();
	void PickUpPass();

	void LoadShaders();
	void BeginMainPass();
	void DrawCube(Shader& shader, glm::mat4 modelMatrix);
	GLuint GetFinalBlurTexture();

	Shader* GetShaderByName(const std::string& name);
	FrameBuffer* GetFrameBufferByName(const std::string& name);
	ShadowMap* GetShadowMapByName(const std::string& name);
	UBO* GetUBOByName(const std::string& name);
	SSBO* GetSSBOByName(const std::string& name);
	Mesh2D* GetQuadMeshByName(const std::string& name);
	std::vector<float>& GetShadowCascadeLevels();
	RendererType GetRenderType();

	// stuff for the editor
	RendererCommon::PostProcessMode GetPostProcessMode();
	void ChangePostProcessMode(RendererCommon::PostProcessMode mode);

	bool BloomEnabled();
	void SetBloom();

	bool ShadowsEnabled();
	void SetShadows();

	float GetExposure();
	void SetExposureValue(float value);

	float GetGammaValue();
	void ChangeGammaValue(float value);

	glm::vec2 GetRenderResolution();
	void SetRenderResolution(int x, int y);

	void Cleanup();
}