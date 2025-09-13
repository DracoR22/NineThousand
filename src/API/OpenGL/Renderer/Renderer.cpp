#include "Renderer.h"
#include <glm/gtx/string_cast.hpp>

namespace OpenGLRenderer {
	RendererType g_rendererType = RendererType::FORWARD;

	std::unordered_map<std::string, Shader> g_Shaders;
	std::unordered_map<std::string, FrameBuffer> g_frameBuffers;
	std::unordered_map<std::string, ShadowMap> g_shadowMaps;
	std::unordered_map<std::string, UBO> g_ubos;
	std::unordered_map<std::string, SSBO> g_ssbos;
	std::unordered_map<std::string, Mesh2D> g_quadMeshes;

	glm::vec2 g_viewportResolution = { 1280 , 720 };
	std::vector<float> g_shadowCascadeLevels{ 500.0f / 50.0f, 500.0f / 25.0f, 500.0f / 10.0f, 500.0f / 2.0f };

	struct RenderData {
		std::vector<CubeMap> cubeMaps = {};

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
		float exposure = 1.0f;;
	} g_renderData;

	bool g_enableBloom = true;
	bool g_castShadows = true;

	///////////////////////////////////////
	void UpdateSSBOS();
	void RenderEnvMap();

	unsigned int g_captureFBO, g_captureRBO;
	unsigned int g_envCubemap;
	unsigned int g_irradianceMap;
	glm::mat4 g_captureProjection = {};
	glm::mat4 g_captureViews[6];
	///////////////////////////////////////

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

		glm::vec2 viewPortResolution = GetRenderResolution();

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

		g_frameBuffers["Refraction"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["Refraction"].Bind();
		g_frameBuffers["Refraction"].CreateAttachment("refractionAttachment", GL_RGBA16F);
		g_frameBuffers["Refraction"].CreateDepthTextureAttachment();
		g_frameBuffers["Refraction"].DrawBuffer();
		g_frameBuffers["Refraction"].Unbind();

		g_frameBuffers["PostProcess"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["PostProcess"].Bind();
		g_frameBuffers["PostProcess"].CreateAttachment("hdrAttachment", GL_RGBA16F);
		g_frameBuffers["PostProcess"].CreateAttachment("emissiveAttachment", GL_RGBA16F);
		g_frameBuffers["PostProcess"].CreateDepthAttachment();
		g_frameBuffers["PostProcess"].DrawBuffers();
		g_frameBuffers["PostProcess"].Unbind();

		g_frameBuffers["FinalImage"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["FinalImage"].Bind();
		g_frameBuffers["FinalImage"].CreateAttachment("colorAttachment", GL_RGBA16F);
		g_frameBuffers["FinalImage"].DrawBuffer();
		g_frameBuffers["FinalImage"].Unbind();

		g_frameBuffers["MSAAPostProcess"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["MSAAPostProcess"].Bind();
		g_frameBuffers["MSAAPostProcess"].CreateMSAAAttachment("msaaAttachment");
		g_frameBuffers["MSAAPostProcess"].DrawBuffer();
		g_frameBuffers["MSAAPostProcess"].Unbind();

		g_frameBuffers["BloomPing"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["BloomPing"].Bind();
		g_frameBuffers["BloomPing"].CreateAttachment("pingColor", GL_RGBA16F);
		g_frameBuffers["BloomPing"].DrawBuffer();
		g_frameBuffers["BloomPing"].Unbind();

		g_frameBuffers["BloomPong"] = FrameBuffer(viewPortResolution.x, viewPortResolution.y);
		g_frameBuffers["BloomPong"].Bind();
		g_frameBuffers["BloomPong"].CreateAttachment("pongColor", GL_RGBA16F);
		g_frameBuffers["BloomPong"].DrawBuffer();
		g_frameBuffers["BloomPong"].Unbind();

		g_shadowMaps["CSM"].InitCSM(int(g_shadowCascadeLevels.size()));

		g_ssbos["Lights"] = SSBO(sizeof(GPULight) * MAX_LIGHTS, GL_DYNAMIC_STORAGE_BIT);
		g_ssbos["LightSpaceMatricesCSM"] = SSBO(sizeof(glm::mat4) * 5, GL_DYNAMIC_STORAGE_BIT);
	}

	void Render() {
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			LoadShaders();
		}

		if (g_rendererType == RendererType::DEFERRED) {
			GBufferPass();
		}
		UpdateSSBOS();
		ShadowPass();
		RefractionPass();
		UpdateFBOs();
		AnimationPass();
		LightingPass();
		WaterPass();
		CubeMapPass();
		DebugPass();
		BillboardPass();
		UIPass();
		BloomPass();
		PostProcessingPass();

		EditorPanel::Render();
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
		g_Shaders["SimpleTexture"].load("simple_texture.vert", "simple_texture.frag");
		g_Shaders["HDRSkybox"].load("hdr_skybox.vert", "hdr_skybox.frag");
		g_Shaders["Irradiance"].load("irradiance_convolution.vert", "irradiance_convolution.frag");
		g_Shaders["EquirectangularToCubemap"].load("equirectangularMap.vert", "equirectangularMap.frag");
		g_Shaders["Bloom"].load("solid_color.vert", "bloom.frag");
		g_Shaders["BlurHorizontal"].load("blur.vert", "blur_horizontal.frag");
		g_Shaders["BlurVertical"].load("blur.vert", "blur_vertical.frag");
	}

	void UpdateSSBOS() {
		SSBO* lightsSSBO = GetSSBOByName("Lights");

		std::vector<LightObject>& sceneLights = Scene::GetLightObjects();
		std::vector<GPULight> gpuLights(sceneLights.size());
		for (size_t i = 0; i < sceneLights.size(); i++) {
			auto& l = sceneLights[i];
			GPULight& g = gpuLights[i];
			g.posX = l.GetPosition().x;
			g.posY = l.GetPosition().y;
			g.posZ = l.GetPosition().z;
			g.radius = l.GetRadius();
			g.strength = l.GetStrength();
			g.colorR = l.GetColor().r;
			g.colorG = l.GetColor().g;
			g.colorB = l.GetColor().b;
			g.type = static_cast<int>(l.GetLightType());
		}

		lightsSSBO->Update(gpuLights.size() * sizeof(GPULight), gpuLights.data());
		lightsSSBO->Bind(0);
	}

	void UpdateFBOs() {
		FrameBuffer* postProcessFBO = GetFrameBufferByName("PostProcess");

		postProcessFBO->Bind();
		
		glViewport(0, 0, GetRenderResolution().x, GetRenderResolution().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// STENCIL STUFF
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void RenderEnvMap() {
		ShadowMap* csmDepth = GetShadowMapByName("CSM");
		Shader* lightingShader = GetShaderByName("Lighting");
		Shader* irradianceShader = GetShaderByName("Irradiance");
	
	/*	Camera* camera = CameraManager::GetActiveCamera();
		std::vector<LightCreateInfo>& sceneLights = GetSceneLights();
		std::vector<float>& shadowCascadeLevels = GetShadowCascadeLevels();

		glViewport(0, 0, 512, 512);
		glBindFramebuffer(GL_FRAMEBUFFER, g_captureFBO);

		for (unsigned int i = 0; i < 6; ++i) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, g_envCubemap, 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			lightingShader->activate();
			lightingShader->setMat4("view", g_captureViews[i]);
			lightingShader->setMat4("projection", g_captureProjection);

			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "lights[" + std::to_string(i) + "]";

				lightingShader->setFloat(lightUniform + ".posX", sceneLights[i].position.x);
				lightingShader->setFloat(lightUniform + ".posY", sceneLights[i].position.y);
				lightingShader->setFloat(lightUniform + ".posZ", sceneLights[i].position.z);
				lightingShader->setFloat(lightUniform + ".radius", sceneLights[i].radius);
				lightingShader->setFloat(lightUniform + ".strength", sceneLights[i].strength);
				lightingShader->setFloat(lightUniform + ".colorR", sceneLights[i].color.r);
				lightingShader->setFloat(lightUniform + ".colorG", sceneLights[i].color.g);
				lightingShader->setFloat(lightUniform + ".colorB", sceneLights[i].color.b);
				lightingShader->setInt(lightUniform + ".type", static_cast<int>(sceneLights[i].type));
			}
			lightingShader->setInt("numLights", sceneLights.size());
			lightingShader->set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);
			lightingShader->setVec3("lightDir", glm::normalize(glm::vec3(20.0f, 50, 20.0f)));
			lightingShader->setFloat("farPlane", camera->GetFarPlane());
			lightingShader->setInt("cascadeCount", shadowCascadeLevels.size());
			for (size_t i = 0; i < shadowCascadeLevels.size(); ++i)
			{
				lightingShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
			}
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D_ARRAY, csmDepth->GetTextureID());
			lightingShader->setInt("shadowMap", 3);
			for (GameObject& gameObject : Scene::GetGameObjects()) {
				if (gameObject.GetModelName() == "Plane" || gameObject.GetModelName() == "Cube") {
					lightingShader->setMat4("model", gameObject.GetModelMatrix());
					lightingShader->setVec2("textureScale", gameObject.GetTextureScale());
					lightingShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(gameObject.GetModelMatrix()))));

					Model* gameObjectModel = AssetManager::GetModelByName(gameObject.GetModelName());
					for (Mesh& mesh : gameObjectModel->m_meshes) {
						int materialIndex = gameObject.GetMeshMaterialIndex(mesh.m_Name);
						Material* meshMaterial = AssetManager::GetMaterialByIndex(materialIndex);
						Texture* baseTexture = AssetManager::GetTextureByIndex(meshMaterial->baseTexture);
						Texture* normalTexture = AssetManager::GetTextureByIndex(meshMaterial->normalTexture);
						Texture* rmaTexture = AssetManager::GetTextureByIndex(meshMaterial->rmaTexture);

						glActiveTexture(GL_TEXTURE0);
						lightingShader->setInt("baseTexture", 0);
						baseTexture->Bind();

						glActiveTexture(GL_TEXTURE1);
						lightingShader->setInt("normalTexture", 1);
						normalTexture->Bind();

						glActiveTexture(GL_TEXTURE2);
						lightingShader->setInt("rmaTexture", 2);
						rmaTexture->Bind();

						glBindVertexArray(mesh.GetVAO());
						glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
						glBindVertexArray(0);

						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

		glViewport(0, 0, 512, 512);
		glBindFramebuffer(GL_FRAMEBUFFER, g_captureFBO);
		Shader* equirectShader = GetShaderByName("EquirectangularToCubemap");
		equirectShader->activate();
		equirectShader->setInt("equirectangularMap", 0);
		equirectShader->setMat4("projection", g_captureProjection);

		Texture* hdrTexture = AssetManager::GetTextureByName("newport_loft.hdr");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture->m_id);

		for (unsigned int i = 0; i < 6; ++i) {
			equirectShader->setMat4("view", g_captureViews[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, g_envCubemap, 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// draw cube
			Model* model = AssetManager::GetModelByName("Cube");

			for (Mesh& mesh : model->m_meshes) {
				glBindVertexArray(mesh.GetVAO());
				glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Resize depth buffer to 32x32 for irradiance capture
		glBindFramebuffer(GL_FRAMEBUFFER, g_captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, g_captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		irradianceShader->activate();
		irradianceShader->setInt("environmentMap", 0);
		irradianceShader->setMat4("projection", g_captureProjection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, g_envCubemap);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, g_captureFBO);
		for (unsigned int i = 0; i < 6; ++i)
		{
			irradianceShader->setMat4("view", g_captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, g_irradianceMap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// draw cube
			Model* model = AssetManager::GetModelByName("Cube");

			for (Mesh& mesh : model->m_meshes) {
				glBindVertexArray(mesh.GetVAO());
				glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DrawCube(Shader& shader, glm::mat4 modelMatrix) {
		Model* model = AssetManager::GetModelByName("Cube");

		for (Mesh& mesh : model->m_meshes) {
			shader.setMat4("model", modelMatrix);
			glBindVertexArray(mesh.GetVAO());
			glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void CubeMapPass() {
		Camera* camera = CameraManager::GetActiveCamera();
		g_renderData.cubeMaps[0].Draw(g_Shaders["Skybox"], CameraManager::GetActiveCamera()->GetViewMatrix(), camera->GetProjectionMatrix());
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

	SSBO* GetSSBOByName(const std::string& name) {
		auto it = g_ssbos.find(std::string{ name });
		return (it != g_ssbos.end()) ? &it->second : nullptr;
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

	bool BloomEnabled() {
		return g_enableBloom;
	}

	void SetBloom() {
		g_enableBloom = !g_enableBloom;
	}

	bool ShadowsEnabled() {
		return g_castShadows;
	}
	void SetShadows() {
		g_castShadows = !g_castShadows;
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

	glm::vec2 GetRenderResolution() {
		return g_viewportResolution;
	}

	void SetRenderResolution(int x, int y) {
		FrameBuffer* postProcessingFrameBuffer = GetFrameBufferByName("PostProcess");
		FrameBuffer* msaaFrameBuffer = GetFrameBufferByName("MSAAPostProcess");

		postProcessingFrameBuffer->Resize(x, y);
		if (g_rendererType == RendererType::FORWARD) {
			msaaFrameBuffer->ResizeMSAA(x, y);
		}

		g_viewportResolution.x = x;
		g_viewportResolution.y = y;
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