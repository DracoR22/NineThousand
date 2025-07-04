#include "Renderer.h"
#include <glm/gtx/string_cast.hpp>

namespace OpenGLRenderer {
	RendererType g_rendererType = RendererType::FORWARD;

	std::unordered_map<std::string, Shader> g_Shaders;
	std::unordered_map<std::string, FrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, ShadowMap> g_shadowMaps;
	std::unordered_map<std::string, UBO> g_ubos;
	std::unordered_map<std::string, Mesh2D> g_quadMeshes;

	std::vector<float> g_shadowCascadeLevels{ 500.0f / 50.0f, 500.0f / 25.0f, 500.0f / 10.0f, 500.0f / 2.0f };
	glm::vec2 g_renderResolution = { 1280, 720 };

	struct RenderData {
		std::vector<LightCreateInfo> sceneLights = {};
		std::vector<CubeMap> cubeMaps = {};

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
		float exposure = 1.0f;;
	} g_renderData;

	void Init() {
		LoadShaders();

		// load skybox
		g_renderData.cubeMaps.clear();
		CubeMap& daySky = g_renderData.cubeMaps.emplace_back();

		std::vector<Texture*> faceTextures = {
			AssetManager::GetTextureByName("SkyRight.jpg"),
			AssetManager::GetTextureByName("SkyLeft.jpg"),
			AssetManager::GetTextureByName("SkyTop.jpg"),
			AssetManager::GetTextureByName("SkyBottom.jpg"),
			AssetManager::GetTextureByName("SkyFront.jpg"),
			AssetManager::GetTextureByName("SkyBack.jpg"),
		};

		std::vector<Texture> faceTexturesCopy;
		for (Texture* texture : faceTextures) {
			if (texture) {
				faceTexturesCopy.push_back(*texture);
			}
		}

		daySky.LoadTextures(faceTexturesCopy);
		daySky.Init();

		// load 2d meshes
		g_quadMeshes["Text"] = Mesh2D(RendererCommon::UI_VERTICES, sizeof(RendererCommon::UI_VERTICES));
		g_quadMeshes["Texture"] = Mesh2D(RendererCommon::UI_VERTICES, sizeof(RendererCommon::UI_VERTICES));
		g_quadMeshes["PostProcess"] = Mesh2D(RendererCommon::POSTPROCESS_QUAD_VERTICES, sizeof(RendererCommon::POSTPROCESS_QUAD_VERTICES));

		// load lights (TODO: MOVE TO SCENE LEVEL)
		LightCreateInfo cubeLampLight;
		cubeLampLight.position = glm::vec3(10.0f, 5.0f, 5.0f);
		cubeLampLight.color = glm::vec3(0.8f);
		cubeLampLight.radius = 70.0f;
		cubeLampLight.strength = 1.0f;
		cubeLampLight.type = LightType::DIRECTIONAL_LIGHT;

		LightCreateInfo cubeLampLight2;
		cubeLampLight2.position = glm::vec3(14.0f, 15.0f, 24.0f);
		cubeLampLight2.color = glm::vec3(0.8f);
		cubeLampLight2.radius = 100.0f;
		cubeLampLight2.strength = 1.0f;
		cubeLampLight2.type = LightType::POINT_LIGHT;

		g_renderData.sceneLights.push_back(cubeLampLight);
	    g_renderData.sceneLights.push_back(cubeLampLight2);
		
		glm::vec2 viewPortResolution = GetRenderResolution();

		// Load frame buffers
		if (g_rendererType == RendererType::DEFERRED) {
			g_frameBuffers["GBuffer"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
			g_frameBuffers["GBuffer"].Bind();
			g_frameBuffers["GBuffer"].CreateAttachment("WorldPosition", GL_RGBA16F);
			g_frameBuffers["GBuffer"].CreateAttachment("BaseColor", GL_RGBA8);
			g_frameBuffers["GBuffer"].CreateAttachment("Normal", GL_RGBA16F);
			g_frameBuffers["GBuffer"].CreateAttachment("RMA", GL_RGBA8);
			g_frameBuffers["GBuffer"].CreateDepthAttachment();
			g_frameBuffers["GBuffer"].DrawBuffers();
			g_frameBuffers["GBuffer"].Unbind();
		}

		g_frameBuffers["Refraction"] = FrameBuffer(Window::m_windowWidth, Window::m_windowHeight);
		g_frameBuffers["Refraction"].Bind();
		g_frameBuffers["Refraction"].CreateAttachment("refractionAttachment", GL_RGBA16F);
		g_frameBuffers["Refraction"].CreateDepthTextureAttachment();
		g_frameBuffers["Refraction"].DrawBuffer();
		g_frameBuffers["Refraction"].Unbind();

		g_frameBuffers["PostProcess"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["PostProcess"].Bind();
		g_frameBuffers["PostProcess"].CreateAttachment("hdrAttachment", GL_RGBA16F);
		g_frameBuffers["PostProcess"].CreateDepthAttachment();
		g_frameBuffers["PostProcess"].DrawBuffer();
		g_frameBuffers["PostProcess"].Unbind();

		g_frameBuffers["MSAA"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["MSAA"].Bind();
		g_frameBuffers["MSAA"].CreateMSAAAttachment("msaaAttachment");
		g_frameBuffers["MSAA"].DrawBuffer();
		g_frameBuffers["MSAA"].Unbind();

		// load shadow maps
		g_shadowMaps["CSM"].InitCSM(int(g_shadowCascadeLevels.size()));

		// load ubos
		g_ubos["CSM"] = UBO(sizeof(glm::mat4) * 16, 0);

	}

	void Render() {
		Camera* camera = CameraManager::GetActiveCamera();
	
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			LoadShaders();
		}

		if (g_rendererType == RendererType::DEFERRED) {
			GBufferPass();
		}
		ShadowPass();
		PreWaterPass();
		UpdateFBOs();
		AnimationPass();
		LightingPass();
		WaterPass();
		g_renderData.cubeMaps[0].Draw(g_Shaders["Skybox"], CameraManager::GetActiveCamera()->GetViewMatrix(), camera->GetProjectionMatrix());
		DebugPass();
		BillboardPass();
		UIPass();
		PostProcessingPass();
	}

	void LoadShaders() {
		g_Shaders["Skybox"].load("skybox.vert", "skybox.frag");
		g_Shaders["ShadowMap"].load("shadow_map.vert", "shadow_map.frag");
		g_Shaders["GBuffer"].load("g_buffer.vert", "g_buffer.frag");
		g_Shaders["CSM"].load("csm_depth.vert", "csm_depth.frag", "csm_depth.geom");
		g_Shaders["Lighting"].load("lighting.vert", "lighting.frag");
		g_Shaders["DeferredLighting"].load("deferred_lighting.vert", "deferred_lighting.frag");
		g_Shaders["Animation"].load("animated.vert", "animated.frag");
		g_Shaders["Water"].load("water.vert", "water.frag");
		g_Shaders["SolidColor"].load("solid_color.vert", "solid_color.frag");
		g_Shaders["OutlineAnimated"].load("outline_animated.vert", "outline_animated.frag");
		g_Shaders["MuzzleFlash"].load("muzzle_flash.vert", "muzzle_flash.frag");
		g_Shaders["UI"].load("ui.vert", "ui.frag");
		g_Shaders["SobelEdges"].load("sobel_edges.vert", "sobel_edges.frag");
		g_Shaders["PostProcess"].load("post_process.vert", "post_process.frag");
		g_Shaders["Instanced"].load("instanced.vert", "instanced.frag");
		g_Shaders["Blur"].load("blur.vert", "blur.frag");
		g_Shaders["SimpleTexture"].load("simple_texture.vert", "simple_texture.frag");
	}

	void UpdateFBOs() {
		FrameBuffer* refractionFrameBuffer = GetFrameBufferByName("Refraction");
		FrameBuffer* msaaFrameBuffer = GetFrameBufferByName("MSAA");

		if (refractionFrameBuffer->GetWidth() != Window::m_windowWidth) {
			refractionFrameBuffer->ResizeRefraction(Window::m_windowWidth, Window::m_windowHeight);
		}

		if (g_rendererType == RendererType::FORWARD) {
			msaaFrameBuffer->Bind();
		}

		glViewport(0, 0, GetRenderResolution().x, GetRenderResolution().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// STENCIL STUFF
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	Shader* GetShaderByName(const std::string& name) {
		auto it = g_Shaders.find(std::string{ name });
		return (it != g_Shaders.end()) ? &it->second : nullptr;
	}

	FrameBuffer* GetFrameBufferByName(const std::string& name) {
		auto it = g_frameBuffers.find(std::string{ name });
		return (it != g_frameBuffers.end()) ? &it->second : nullptr;
	}

	ShadowMap* GetShadowMapByName(const std::string& name) {
		auto it = g_shadowMaps.find(std::string{ name });
		return (it != g_shadowMaps.end()) ? &it->second : nullptr;
	}

	UBO* GetUBOByName(const std::string& name) {
		auto it = g_ubos.find(std::string{ name });
		return (it != g_ubos.end()) ? &it->second : nullptr;
	}

	Mesh2D* GetQuadMeshByName(const std::string& name) {
		auto it = g_quadMeshes.find(std::string{ name });
		return (it != g_quadMeshes.end()) ? &it->second : nullptr;
	}

	std::vector<float>& GetShadowCascadeLevels() {
		return g_shadowCascadeLevels;
	}

	RendererType GetRenderType() {
		return g_rendererType;
	}

	RendererCommon::PostProcessMode GetPostProcessMode() {
		return g_renderData.currentMode;
	}

	void ChangePostProcessMode(RendererCommon::PostProcessMode mode) {
		g_renderData.currentMode = mode;
	}

	float GetExposure() {
		return g_renderData.exposure;
	}

	void SetExposureValue(float value) {
		g_renderData.exposure = value;
	}

	float GetGammaValue() {
		return g_renderData.gamma;
	}
	void ChangeGammaValue(float value) {
		g_renderData.gamma = value;
	}

	std::vector<LightCreateInfo>& GetSceneLights() {
		return g_renderData.sceneLights;
	}

	void UpdateLightStrength(int index, float strength) {
		g_renderData.sceneLights[index].strength = strength;
	}

	void UpdateLightRadius(int index, float radius) {
		g_renderData.sceneLights[index].radius = radius;
	}

	void UpdateLightPosition(int index, glm::vec3 newPosition) {
		g_renderData.sceneLights[index].position = newPosition;
	}

	glm::vec2 GetRenderResolution() {
		return g_renderResolution;
	}

	void SetRenderResolution(int x, int y) {
		FrameBuffer* postProcessingFrameBuffer = GetFrameBufferByName("PostProcess");
		FrameBuffer* msaaFrameBuffer = GetFrameBufferByName("MSAA");

		postProcessingFrameBuffer->Resize(x, y);
		if (g_rendererType == RendererType::FORWARD) {
			msaaFrameBuffer->ResizeMSAA(x, y);
		}

		g_renderResolution.x = x;
		g_renderResolution.y = y;
	}

	void Cleanup() {
		for (CubeMap cubeMap : g_renderData.cubeMaps) {
			cubeMap.Cleanup();
		};

		for (auto& [name, framebuffer] : g_frameBuffers) {
			framebuffer.Cleanup();
		}

		for (auto& [name, quadMesh] : g_quadMeshes) {
			quadMesh.Cleanup();
		}

		for (auto& [name, shadowMap] : g_shadowMaps) {
			shadowMap.Cleanup();
		}
	}
}