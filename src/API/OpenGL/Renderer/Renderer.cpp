#include "Renderer.h"
#include <glm/gtx/string_cast.hpp>

namespace OpenGLRenderer {

	RendererType g_rendererType = RendererType::FORWARD;

	std::unordered_map<std::string, Shader> g_Shaders;
	std::unordered_map<std::string, FrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, ShadowMap> g_shadowMaps;
	std::unordered_map<std::string, UBO> g_ubos;

	ShadowMap g_shadowMap;

	struct Shaders {
		Shader sobelEdgesShader;
		Shader postProcessShader;
		Shader solidColorShader;
		Shader instancedShader;
		Shader uiShader;
		Shader blurShader;
		Shader muzzleFlashShader;
		Shader simpleTextureShader;
		Shader outlineAnimatedShader;
	} g_shaders;

	struct RenderData {
		unsigned int frameBufferQuadVAO = 0;
		unsigned int frameBufferQuadVBO = 0;

		std::vector<LightCreateInfo> sceneLights = {};
		std::vector<CubeMap> cubeMaps = {};

		
		ShadowMap cubeMapShadowMap;

		Mesh2D textMesh;
		Mesh2D crossHairMesh;
		Mesh2D muzzleFlashMesh;

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
		float exposure = 1.0f;;
	} g_renderData;


	std::vector<float> g_shadowCascadeLevels{ 500.0f / 50.0f, 500.0f / 25.0f, 500.0f / 10.0f, 500.0f / 2.0f };
	glm::vec2 g_renderResolution = { 1280, 720 };

	void Init() {
		Player& player = Game::GetPLayerByIndex(0);

		// load shaders
		g_shaders.sobelEdgesShader.load("sobel_edges.vert", "sobel_edges.frag");
		g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
		g_shaders.solidColorShader.load("solid_color.vert", "solid_color.frag");
		g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
		g_shaders.uiShader.load("ui.vert", "ui.frag");
		g_shaders.blurShader.load("blur.vert", "blur.frag");
		g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
		g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");
		g_shaders.outlineAnimatedShader.load("outline_animated.vert", "outline_animated.frag");

	/*	
		g_Shaders["Animation"].load("animated.vert", "animated.frag");
		g_Shaders["SobelEdges"].load("sobel_edges.vert", "sobel_edges.frag");
		g_Shaders["PostProcess"].load("post_process.vert", "post_process.frag");
		g_Shaders["SolidColor"].load("solid_color.vert", "solid_color.frag");
		g_Shaders["Instanced"].load("instanced.vert", "instanced.frag");
		g_Shaders["UI"].load("ui.vert", "ui.frag");
		g_Shaders["Blur"].load("blur.vert", "blur.frag");
		g_Shaders["MuzzleFlash"].load("muzzle_flash.vert", "muzzle_flash.frag");
		g_Shaders["SimpleTexture"].load("simple_texture.vert", "simple_texture.frag");
		g_Shaders["OutlineAnimated"].load("outline_animated.vert", "outline_animated.frag");
		g_Shaders["ShadowCubeMap"].load("shadow_cube_map.vert", "shadow_cube_map.frag", "shadow_cube_map.geom");
		*/

		g_Shaders["Skybox"].load("skybox.vert", "skybox.frag");
		g_Shaders["ShadowMap"].load("shadow_map.vert", "shadow_map.frag");
		g_Shaders["GBuffer"].load("g_buffer.vert", "g_buffer.frag");
		g_Shaders["CSM"].load("csm_depth.vert", "csm_depth.frag", "csm_depth.geom");
		g_Shaders["Lighting"].load("lighting.vert", "lighting.frag");
		g_Shaders["DeferredLighting"].load("deferred_lighting.vert", "deferred_lighting.frag");
		g_Shaders["Animation"].load("animated.vert", "animated.frag");
		g_Shaders["Water"].load("water.vert", "water.frag");

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
		g_renderData.textMesh.Create();
		g_renderData.crossHairMesh.Create();
		g_renderData.muzzleFlashMesh.Create();

		// Quad For FrameBuffer
		glGenVertexArrays(1, &g_renderData.frameBufferQuadVAO);
		glGenBuffers(1, &g_renderData.frameBufferQuadVBO);

		glBindVertexArray(g_renderData.frameBufferQuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, g_renderData.frameBufferQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RendererCommon::QUAD_VERTICES), RendererCommon::QUAD_VERTICES, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);

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
		// Hotload shaders
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			g_shaders.sobelEdgesShader.load("sobel_edges.vert", "sobel_edges.frag");
			g_shaders.solidColorShader.load("solid_color.vert", "solid_color.frag");
			g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
			g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
			g_shaders.blurShader.load("blur.vert", "blur.frag");
			g_shaders.uiShader.load("ui.vert", "ui.frag");
			g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
			g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");
			g_shaders.outlineAnimatedShader.load("outline_animated.vert", "outline_animated.frag");
		}

		Player& player = Game::GetPLayerByIndex(0);
		glm::mat4 projectionMatrix = CameraManager::GetActiveCamera()->GetProjectionMatrix();
		glm::mat4 viewMatrix = CameraManager::GetActiveCamera()->GetViewMatrix();

		Frustum camFrustum = CameraManager::GetActiveCamera()->GetFrustum();
		Camera* camera = CameraManager::GetActiveCamera();

		Shader* lightingShader = GetShaderByName("Lighting");
		
		if (g_rendererType == RendererType::DEFERRED) {
			GBufferPass();
		}
		ShadowPass();
		PreWaterPass();
		UpdateFBOs();
		AnimationPass();
		LightingPass();
		WaterPass();

		ShadowMap* csmDepth = GetShadowMapByName("CSM");
		FrameBuffer* refractionFrameBuffer = GetFrameBufferByName("Refraction");

		FrameBuffer* postProcessingFrameBuffer = GetFrameBufferByName("PostProcess");
		FrameBuffer* msaaFrameBuffer = GetFrameBufferByName("MSAA");

		// DEBUG LIGHTS
		g_shaders.solidColorShader.activate();
		g_shaders.solidColorShader.set3Float("viewPos", CameraManager::GetActiveCamera()->cameraPos);
		g_shaders.solidColorShader.setMat4("view", viewMatrix);
		g_shaders.solidColorShader.setMat4("projection", projectionMatrix);
		g_shaders.solidColorShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		glm::mat4 lmodel = glm::mat4(1.0f);
		lmodel = glm::translate(lmodel, glm::vec3(10.0f, 5.0f, 5.0f));
		lmodel = glm::scale(lmodel, glm::vec3(0.75f));
		lmodel *= glm::mat4(1.0f);
		g_shaders.solidColorShader.setMat4("model", lmodel);
		AssetManager::DrawModel("CubeLamp", g_shaders.solidColorShader);


		// ------ INSTANCE PASS ----------
		/*g_shaders.instancedShader.activate();
		g_shaders.instancedShader.setMat4("view", view);
		g_shaders.instancedShader.setMat4("projection", projection);*/

		//AssetManager::DrawModelInstanced("Bullet", g_shaders.instancedShader, bulletCreateInfo.instanceOffsets);	

		// ------ CUBEMAP PASS -------------
		g_renderData.cubeMaps[0].Draw(g_Shaders["Skybox"], CameraManager::GetActiveCamera()->GetViewMatrix(), projectionMatrix);

		// OUTLINE PASS
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		g_shaders.outlineAnimatedShader.activate();
		g_shaders.outlineAnimatedShader.setMat4("view", viewMatrix);
		g_shaders.outlineAnimatedShader.setMat4("projection", projectionMatrix);
		g_shaders.outlineAnimatedShader.setFloat("outlineThickness", 0.02f);

		static float accumulatedTime = 0.0f;
		accumulatedTime += Window::GetDeltaTime();
		g_shaders.outlineAnimatedShader.setFloat("time", accumulatedTime);

		for (GameObject& outlineObject : Scene::GetGameObjects()) {
			if (!outlineObject.IsSelected()) {
				continue;
			}

			float baseScale = glm::length(outlineObject.GetSize()) / sqrt(3.0f);
			float thicknessMultiplier = glm::clamp(2.0f / baseScale, 1.0f, 1.1f);

			float outlineScale = 1.01f * thicknessMultiplier;

			glm::mat4 stModel = glm::mat4(1.0f);

			stModel = glm::translate(stModel, outlineObject.GetPosition());

			stModel = glm::scale(stModel, outlineObject.GetSize() * outlineScale);
			stModel *= outlineObject.GetRotationMatrix();

			g_shaders.outlineAnimatedShader.setMat4("model", stModel);
			AssetManager::DrawModel(outlineObject.GetModelName(), g_shaders.outlineAnimatedShader);
		}


		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);

		// DEBUG PASS
		static bool drawCollisionBoxes = false;

		if (Keyboard::KeyJustPressed(GLFW_KEY_F3)) {
			drawCollisionBoxes = !drawCollisionBoxes;
		}

		if (drawCollisionBoxes) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_DEPTH_TEST);

			g_shaders.solidColorShader.activate();
			g_shaders.solidColorShader.set3Float("viewPos", CameraManager::GetActiveCamera()->cameraPos);
			g_shaders.solidColorShader.setMat4("view", viewMatrix);
			g_shaders.solidColorShader.setMat4("projection", projectionMatrix);
			g_shaders.solidColorShader.setVec3("lightColor", 0.0f, 1.0f, 0.9f);

			for (GameObject& gameObject : Scene::GetGameObjects()) {
				if (gameObject.GetPhysicsId() != 0) {
					RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(gameObject.GetPhysicsId());
					RigidStatic* rigidStatic = Physics::GetRigidStaticById(gameObject.GetPhysicsId());

					if (rigidStatic) {
						PxBounds3 bounds = rigidStatic->GetPxRigidStatic()->getWorldBounds();
						PxVec3 center = (bounds.minimum + bounds.maximum) * 0.5f;
						PxVec3 extents = (bounds.maximum - bounds.minimum) * 0.5f;

						glm::vec3 glmCenter = Physics::PxVec3toGlmVec3(center);
						glm::vec3 glmExtents = Physics::PxVec3toGlmVec3(extents);

						/*glm::mat4 dptModel = glm::mat4(1.0f);
						dptModel = glm::translate(dptModel, rigidStatic->GetCurrentPosition());
						dptModel = glm::scale(dptModel, glmExtents * 2.0f);
						dptModel *= glm::mat4_cast(rigidStatic->GetCurrentRotation());*/

						gameObject.SetSize(glmExtents * 2.0f);
						gameObject.SetPosition(glmCenter);
						g_shaders.solidColorShader.setMat4("model", gameObject.GetModelMatrix());

						Model* debugModel = AssetManager::GetModelByName(gameObject.GetModelName());

						for (unsigned int i = 0; i < debugModel->meshes.size(); i++) {
							glBindVertexArray(debugModel->meshes[i].GetVAO());
							glDrawElements(GL_TRIANGLES, debugModel->meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0);
						}
					}

					if (rigidDynamic) {
						PxBounds3 bounds = rigidDynamic->GetPxRigidDynamic()->getWorldBounds();

						PxVec3 center = (bounds.minimum + bounds.maximum) * 0.5f;
						PxVec3 extents = (bounds.maximum - bounds.minimum) * 0.5f;

						glm::vec3 glmCenter = Physics::PxVec3toGlmVec3(center);
						glm::vec3 glmExtents = Physics::PxVec3toGlmVec3(extents);

						/*glm::mat4 dptModel = glm::mat4(1.0f);
						dptModel = glm::translate(dptModel, rigidDynamic->GetCurrentPosition());
						dptModel = glm::scale(dptModel, glmExtents * 2.0f);
						dptModel *= glm::mat4_cast(rigidDynamic->GetCurrentRotation());*/

						g_shaders.solidColorShader.setMat4("model", gameObject.GetModelMatrix());

						Model* debugModel = AssetManager::GetModelByName(gameObject.GetModelName());

						for (unsigned int i = 0; i < debugModel->meshes.size(); i++) {
							glBindVertexArray(debugModel->meshes[i].GetVAO());
							glDrawElements(GL_TRIANGLES, debugModel->meshes[i].GetIndices().size(), GL_UNSIGNED_INT, 0);
						}
					}
				}
			}

			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


		// ------ MUZZLE FLASH PASS
		glm::vec3 barrelOffset = player.GetEquipedWeaponInfo()->muzzleFlashOffset;
		glm::mat4 gunTransform = glm::translate(glm::mat4(1.0f), player.m_currentWeaponGameObject.GetPosition()) * player.m_currentWeaponGameObject.GetRotationMatrix();

		glm::vec4 worldBarrelPos = gunTransform * glm::vec4(barrelOffset, 1.0f);

		glm::mat4 muzzleFlashModel = glm::mat4(1.0f);

		muzzleFlashModel = glm::translate(muzzleFlashModel, glm::vec3(worldBarrelPos));

		muzzleFlashModel *= glm::inverse(glm::mat4(glm::mat3(viewMatrix)));
		muzzleFlashModel = glm::scale(muzzleFlashModel, glm::vec3(4.0f));

		float randomAngle = Utils::RandomFloat(0.0f, glm::two_pi<float>());

		g_shaders.muzzleFlashShader.activate();

		g_shaders.muzzleFlashShader.setMat4("model", muzzleFlashModel);
		g_shaders.muzzleFlashShader.setMat4("view", viewMatrix);
		g_shaders.muzzleFlashShader.setMat4("projection", projectionMatrix);
		g_shaders.muzzleFlashShader.setFloat("rotation", randomAngle);

		Texture* muzzleFlashTexture = AssetManager::GetTextureByName("MuzzleFlash.png");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, muzzleFlashTexture->m_id);

		if (player.m_muzzleFlashTimer > 0) {
			g_renderData.muzzleFlashMesh.RenderTexture(g_shaders.muzzleFlashShader);
			player.m_muzzleFlashTimer--;
		}

		// ------ UI PASS -------------
		glDisable(GL_DEPTH_TEST);
		glm::mat4 UiProjection = glm::ortho(0.0f, (float)Window::m_windowWidth, (float)Window::m_windowHeight, 0.0f);
		Texture* sansFontTexture = AssetManager::GetTextureByName("sans.png");

		g_shaders.uiShader.activate();
		for (UITextElement& uiTextElement : UIManager::GetTextElements()) {
			g_shaders.uiShader.setMat4("projection", UiProjection);
			g_shaders.uiShader.setMat4("model", glm::mat4(1.0f));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sansFontTexture->m_id);

			g_renderData.textMesh.RenderText(uiTextElement.text, uiTextElement.posX, uiTextElement.posY, uiTextElement.size, uiTextElement.fontColor, g_shaders.uiShader);
		}

		// CROSSHAIR DOT PASS
		UIElement crosshairElement;
		crosshairElement.m_size = 50.0f;
		crosshairElement.m_useAligment = true;
		crosshairElement.m_alignment = UIAlignment::Center;

		g_shaders.uiShader.activate();
		g_shaders.uiShader.setMat4("projection", UiProjection);
		g_shaders.uiShader.setMat4("model", crosshairElement.GetModelMatrix());
		g_shaders.uiShader.setInt("baseTexture", 0);

		Texture* crosshairTexture = AssetManager::GetTextureByName("CrossHairDotOutline.png");

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, crosshairTexture->m_id);
		g_renderData.crossHairMesh.RenderTexture(g_shaders.uiShader);


		glEnable(GL_DEPTH_TEST);


		// ------ POST PROCESS PASS -----------
		if (g_rendererType == RendererType::FORWARD) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFrameBuffer->GetFBO());
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessingFrameBuffer->GetFBO());
		if (postProcessingFrameBuffer->GetWidth() == GetRenderResolution().x) {
			glBlitFramebuffer(0, 0, GetRenderResolution().x, GetRenderResolution().y, 0, 0, GetRenderResolution().x, GetRenderResolution().y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::m_windowWidth, Window::m_windowHeight);


		switch (g_renderData.currentMode) {
		case RendererCommon::PostProcessMode::NONE:
			g_shaders.postProcessShader.activate();
			g_shaders.postProcessShader.setInt("screenTexture", 0);
			g_shaders.postProcessShader.setInt("bloomTexture", 1);
			g_shaders.postProcessShader.setFloat("gamma", g_renderData.gamma);
			g_shaders.postProcessShader.setFloat("exposure", g_renderData.exposure);

			break;
		case RendererCommon::PostProcessMode::SHARPEN:
			g_shaders.sobelEdgesShader.activate();
			g_shaders.sobelEdgesShader.setInt("screenTexture", 0);
			break;
		default:
			g_shaders.postProcessShader.activate();
			g_shaders.postProcessShader.setInt("screenTexture", 0);
			g_shaders.postProcessShader.setInt("bloomTexture", 1);
			break;
		}

		glBindVertexArray(g_renderData.frameBufferQuadVAO);
		glDisable(GL_DEPTH_TEST);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postProcessingFrameBuffer->GetColorAttachmentTextureIdByIndex(0));

		glDrawArrays(GL_TRIANGLES, 0, 6);
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
		g_shadowMap.Cleanup();
		g_renderData.textMesh.Cleanup();

		for (CubeMap cubeMap : g_renderData.cubeMaps) {
			cubeMap.Cleanup();
		};

		for (auto& [name, framebuffer] : g_frameBuffers) {
			framebuffer.Cleanup();
		}
	}
}