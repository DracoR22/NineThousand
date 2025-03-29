#include "Renderer.h"

namespace OpenGLRenderer {

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct Shaders {
		Shader texturedObjectShader;
		Shader skyboxShader;
		Shader animShader;
		Shader lampShader;
		Shader shadowMapShader;
		Shader weaponShader;
		Shader sharpenShader;
		Shader postProcessShader;
		Shader testShader;
		Shader instancedShader;
		Shader uiShader;
	} _shaders;

	ModelCreateInfo bulletCreateInfo{
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f),
			{                                     // instanceOffsets (hardcoded)
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 0.0f, 0.0f),
		glm::vec3(-2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, -2.0f, 0.0f)
	}
	};

	struct RenderData {
		unsigned int frameBufferQuadVAO = 0;
		unsigned int frameBufferQuadVBO = 0;

		std::vector<PointLight> sceneLights = {};
		std::vector<CubeMap> cubeMaps = {};

		ShadowMap shadowMap;
		Mesh2D textMesh;

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
		float exposure = 1.0f;
	} g_renderData;

	struct RenderFrameBuffer {
		FrameBuffer postProcessingFrameBuffer;
		FrameBuffer mssaFrameBuffer;
	} g_renderFrameBuffers;

	std::vector<glm::vec3> generateGridOffsets(const glm::vec3& basePosition, int rows, int cols, float spacing) {
		std::vector<glm::vec3> offsets;
		for (int y = 0; y < rows; ++y) {
			for (int x = 0; x < cols; ++x) {
				glm::vec3 offset = glm::vec3(x * spacing, 0.0f, y * spacing);
				offsets.push_back(basePosition + offset);
			}
		}
		return offsets;
	}

	void Init() {

		// load shaders
		_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
		_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
		_shaders.animShader.load("animated.vert", "animated.frag");
		_shaders.lampShader.load("lamp.vert", "lamp.frag");
		_shaders.shadowMapShader.load("shadow_map.vert", "shadow_map.frag");
		_shaders.weaponShader.load("weapon.vert", "weapon.frag");
		_shaders.sharpenShader.load("sharpen.vert", "sharpen.frag");
		_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
		_shaders.testShader.load("test.vert", "test.frag");
		_shaders.instancedShader.load("instanced.vert", "instanced.frag");
		_shaders.uiShader.load("ui.vert", "ui.frag");

		// load skybox
		g_renderData.cubeMaps.clear();

		CubeMap& daySky = g_renderData.cubeMaps.emplace_back();
		std::vector<std::string> daySkyFaces{
			"resources/textures/SkyRight.jpg",
			"resources/textures/SkyLeft.jpg",
			"resources/textures/SkyTop.jpg",
			"resources/textures/SkyBottom.jpg",
			"resources/textures/SkyFront.jpg",
			"resources/textures/SkyBack.jpg",
		};
		daySky.loadTextures(daySkyFaces);
		daySky.init();

		// load text
		g_renderData.textMesh.Create();


		// load models
		ModelCreateInfo glockCreateInfo{
		 glm::vec3(0.0f, 3.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo p90CreateInfo{
		 glm::vec3(0.0f, 5.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo aks74uCreateInfo{
		 glm::vec3(0.0f, 5.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo cubeCreateInfo{
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo lampCreateInfo{
			glm::vec3(10.0f, 5.0f, 5.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo planeCreateInfo{
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::mat4(1.0f)
		};

		

		/*bulletCreateInfo.instanceOffsets = generateGridOffsets(bulletCreateInfo.position, 5, 5, 1.5f);*/

		AssetManager::LoadAssimpModel("P90", "resources/models/P90T.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);
		AssetManager::LoadAssimpModel("AKS74U", "resources/models/AKS74UBfbx.fbx", aks74uCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);
		AssetManager::LoadModel("Bullet", ModelType::CUBE, bulletCreateInfo);

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

		PointLight cubeLampLight;
		cubeLampLight.position = AssetManager::GetModelByName("CubeLamp")->pos;
		cubeLampLight.constant = 1.0f;
		cubeLampLight.linear = 0.0014f;
		cubeLampLight.quadratic = 0.000007f;
		cubeLampLight.ambient = glm::vec3(0.02f);
		cubeLampLight.diffuse = glm::vec3(0.8f);
		cubeLampLight.specular = glm::vec3(1.0f);

		g_renderData.sceneLights.push_back(cubeLampLight);

		// Load frame buffers
		g_renderFrameBuffers.postProcessingFrameBuffer.Create(Window::currentWidth, Window::currentHeight);
		g_renderFrameBuffers.postProcessingFrameBuffer.CreateAttachment();

		g_renderFrameBuffers.mssaFrameBuffer.Create(Window::currentWidth, Window::currentHeight);
		g_renderFrameBuffers.mssaFrameBuffer.CreateMSAAAttachment();

		// Load shadow map
		g_renderData.shadowMap.Init();

		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");
		Model* aks74uModel = AssetManager::GetModelByName("AKS74U");

		// load animations
		AssetManager::LoadAnimation("AKS74U_Idle", "resources/animations/AKS74U_Idle.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_Reload", "resources/animations/AKS74U_Reload.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_Walk", "resources/animations/AKS74U_Walk.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_Fire0", "resources/animations/AKS74U_Fire0.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_Draw", "resources/animations/AKS74U_Draw.fbx", aks74uModel);

		AssetManager::LoadAnimation("Glock_Idle", "resources/animations/Glock_Idle.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Reload", "resources/animations/Glock_Reload.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ReloadEmpty", "resources/animations/Glock_ReloadEmpty.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Walk", "resources/animations/Glock_Walk.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Fire0", "resources/animations/Glock_Fire0.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Draw", "resources/animations/Glock_Draw.fbx", glockModel);

		AssetManager::LoadAnimation("P90_Idle", "resources/animations/P90_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Reload", "resources/animations/P90_Reload.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Walk", "resources/animations/P90_Walk.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Fire0", "resources/animations/P90_Fire0.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Draw", "resources/animations/P90_Draw.fbx", p90Model);

		

		AssetManager::LoadAnimator("GlockAnimator", AssetManager::GetAnimationByName("Glock_Idle"));
		AssetManager::LoadAnimator("P90Animator", AssetManager::GetAnimationByName("P90_Idle"));
		AssetManager::LoadAnimator("AKS74UAnimator", AssetManager::GetAnimationByName("AKS74U_Idle"));
	}

	void RenderFrame() {
		// Hotload shaders
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
			_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
			_shaders.animShader.load("animated.vert", "animated.frag");
			_shaders.lampShader.load("lamp.vert", "lamp.frag");
			_shaders.weaponShader.load("weapon.vert", "weapon.frag");
			_shaders.sharpenShader.load("sharpen.vert", "sharpen.frag");
			_shaders.testShader.load("test.vert", "test.frag");
			_shaders.instancedShader.load("instanced.vert", "instanced.frag");
			_shaders.postProcessShader.load("post_process.vert", "post_process.frag");

		}

		Player& player = Game::GetPLayerByIndex(0);

		Animator* glockAnimator = AssetManager::GetAnimatorByName("GlockAnimator");
		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");
		Animator* aks74uAnimator = AssetManager::GetAnimatorByName("AKS74UAnimator");
		

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = player.camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.5f, 500.0f);

		// ------ SHADOW PASS (Render to Depth Map) ------
		glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);

		glm::mat4 lightView = glm::lookAt(g_renderData.sceneLights[0].position, AssetManager::GetModelByName("Cube")->pos, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = orthogonalProjection * lightView;

		g_renderData.shadowMap.Clear();

		glCullFace(GL_FRONT);

		_shaders.shadowMapShader.activate();
		_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);
		AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

		glCullFace(GL_BACK);

		g_renderData.shadowMap.Unbind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------ BIND FRAME BUFFERS -------
		if (g_renderFrameBuffers.postProcessingFrameBuffer.GetWidth() != Window::currentWidth || g_renderFrameBuffers.postProcessingFrameBuffer.GetHeight() != Window::currentHeight) {
			g_renderFrameBuffers.postProcessingFrameBuffer.Resize(Window::currentWidth, Window::currentHeight);
		}

		g_renderFrameBuffers.mssaFrameBuffer.Bind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------ RENDER PASS ------
		_shaders.weaponShader.activate();
		_shaders.weaponShader.setMat4("view", view);
		_shaders.weaponShader.setMat4("projection", projection);
		_shaders.weaponShader.setMat4("lightProjection", lightProjection);
		_shaders.weaponShader.setInt("noPointLights", g_renderData.sceneLights.size());
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

			_shaders.weaponShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			_shaders.weaponShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			_shaders.weaponShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			_shaders.weaponShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);

			_shaders.weaponShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			_shaders.weaponShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			_shaders.weaponShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
		}
		if (player.GetEquipedWeaponInfo()->name == "Glock") {
			auto transforms = glockAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("Glock", _shaders.weaponShader);
		}
		/*else if (player.GetEquipedWeaponInfo()->name == "AKS74U") {
			auto transforms = aks74uAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}

			AssetManager::DrawModel("AKS74U", _shaders.weaponShader);
		}*/
		
		else if (player.GetEquipedWeaponInfo()->name == "P90") {
			auto transforms = p90Animator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("P90", _shaders.weaponShader);
		}

		_shaders.animShader.activate();
		_shaders.animShader.setMat4("view", view);
		_shaders.animShader.setMat4("projection", projection);
		if (player.GetEquipedWeaponInfo()->name == "AKS74U") {
			auto transforms = aks74uAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}

			AssetManager::DrawModel("AKS74U", _shaders.animShader);
		}

		/*_shaders.weaponShader.activate();
		_shaders.weaponShader.setMat4("view", view);
		_shaders.weaponShader.setMat4("projection", projection);
		_shaders.weaponShader.setMat4("lightProjection", lightProjection);
		_shaders.weaponShader.setInt("noPointLights", g_renderData.sceneLights.size());
		_shaders.weaponShader.setFloat("gamma", g_renderData.gamma);
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

			_shaders.weaponShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			_shaders.weaponShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			_shaders.weaponShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			_shaders.weaponShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);

			_shaders.weaponShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			_shaders.weaponShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			_shaders.weaponShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
		}
		
			auto transforms = aks74uAnimator->GetFinalBoneMatrices();
			
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("AKS74U", _shaders.weaponShader);*/

		

		_shaders.texturedObjectShader.activate();
		_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
		_shaders.texturedObjectShader.setMat4("view", view);
		_shaders.texturedObjectShader.setMat4("projection", projection);
		_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
		_shaders.texturedObjectShader.setInt("noPointLights", g_renderData.sceneLights.size());
		_shaders.texturedObjectShader.setInt("shadowMap", 3);
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

			_shaders.texturedObjectShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);

			_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
		}

		/*Scene::GetPrimitiveModelByName("Plane")->draw(_shaders.texturedObjectShader);*/
		AssetManager::DrawModel("Plane", _shaders.texturedObjectShader);
		


		_shaders.texturedObjectShader.activate();
		_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
		_shaders.texturedObjectShader.setMat4("view", view);
		_shaders.texturedObjectShader.setMat4("projection", projection);
		_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
		_shaders.texturedObjectShader.setInt("noPointLights", g_renderData.sceneLights.size());
		_shaders.texturedObjectShader.setInt("shadowMap", 3);
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

			_shaders.texturedObjectShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);

			_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
		}
		AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

		_shaders.lampShader.activate();
		_shaders.lampShader.set3Float("viewPos", player.getPosition());
		_shaders.lampShader.setMat4("view", view);
		_shaders.lampShader.setMat4("projection", projection);
		_shaders.lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		AssetManager::DrawModel("CubeLamp", _shaders.lampShader);

		// ------ BULLET PASS ----------
		_shaders.instancedShader.activate();
		_shaders.instancedShader.setMat4("view", view);
		_shaders.instancedShader.setMat4("projection", projection);

		AssetManager::DrawModelInstanced("Bullet", _shaders.instancedShader, bulletCreateInfo.instanceOffsets);

		/*for (int i = 0; i < 5; i++) {
			Model* bulletModel = AssetManager::GetModelByName("Bullet");

			bulletModel->setPosition(glm::vec3(i * 2.0f, 1.0f, 0.0f));

			AssetManager::DrawModel("Bullet", _shaders.testShader);
		}*/		

		// ------ CUBEMAP PASS -------------
		g_renderData.cubeMaps[0].render(_shaders.skyboxShader, player.camera.getViewMatrix(), projection);

		// ------ UI PASS -------------
		glDisable(GL_DEPTH_TEST);
		glm::mat4 UiProjection = glm::ortho(0.0f, (float)Window::currentWidth, (float)Window::currentHeight, 0.0f);
		_shaders.uiShader.activate();
		_shaders.uiShader.setMat4("projection", UiProjection);

		float fpsTextX = 10.0f;  // Small offset from the left side
		float fpsTextY = 30.0f;
		float debugFontSize = 0.2f;  

		float posTextX = 10.0f;
		float posTextY = 50.0f;

		g_renderData.textMesh.RenderUI("FPS: " + std::to_string(Window::GetFPSCount()), fpsTextX, fpsTextY, debugFontSize, glm::vec3(1.0f, 1.0f, 1.0f), _shaders.uiShader);

		std::string playerPosText = "Player Position: (" +
			std::to_string(player.getPosition().x) + ", " +
			std::to_string(player.getPosition().y) + ", " +
			std::to_string(player.getPosition().z) + ")";

		g_renderData.textMesh.RenderUI(playerPosText, posTextX, posTextY, debugFontSize, glm::vec3(1.0f, 1.0f, 1.0f), _shaders.uiShader);


		glEnable(GL_DEPTH_TEST);


		// ------ FRAME BUFFER PASS -----------
		glBindFramebuffer(GL_READ_FRAMEBUFFER, g_renderFrameBuffers.mssaFrameBuffer.GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_renderFrameBuffers.postProcessingFrameBuffer.GetFBO());
		glBlitFramebuffer(0, 0, Window::currentWidth, Window::currentHeight, 0, 0, Window::currentWidth, Window::currentHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		switch (g_renderData.currentMode)
		{
		case RendererCommon::PostProcessMode::NONE:
			_shaders.postProcessShader.activate();
			_shaders.postProcessShader.setInt("screenTexture", 0);
			_shaders.postProcessShader.setFloat("gamma", g_renderData.gamma);
			_shaders.postProcessShader.setFloat("exposure", g_renderData.exposure);
			break;
		case RendererCommon::PostProcessMode::SHARPEN:
			_shaders.sharpenShader.activate();
			_shaders.sharpenShader.setInt("screenTexture", 0);
			break;
		default:
			_shaders.postProcessShader.activate();
			_shaders.postProcessShader.setInt("screenTexture", 0);
			break;
		}

		glBindVertexArray(g_renderData.frameBufferQuadVAO);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		g_renderFrameBuffers.postProcessingFrameBuffer.BindTexture();

		glDrawArrays(GL_TRIANGLES, 0, 6);
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

	void Cleanup() {
		g_renderData.shadowMap.Cleanup();
		g_renderData.textMesh.Cleanup();

		for (CubeMap cubeMap : g_renderData.cubeMaps) {
			cubeMap.cleanup();
		};

		g_renderFrameBuffers.postProcessingFrameBuffer.Cleanup();
		g_renderFrameBuffers.mssaFrameBuffer.Cleanup();
	}
}