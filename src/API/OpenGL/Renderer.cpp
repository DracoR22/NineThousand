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
	} _shaders;

	struct RenderData {
		unsigned int frameBufferQuadVAO = 0;
		unsigned int frameBufferQuadVBO = 0;

		std::vector<PointLight> sceneLights = {};
		std::vector<FrameBuffer> frameBuffers = {};
		std::vector<CubeMap> cubeMaps = {};

		ShadowMap shadowMap;

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
	} g_renderData;

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

		AssetManager::LoadAssimpModel("P90", "resources/models/P90T.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);
		AssetManager::LoadModel("Bullet", ModelType::CUBE, cubeCreateInfo);

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
		g_renderData.frameBuffers.clear();

		FrameBuffer& frameBuffer = g_renderData.frameBuffers.emplace_back();
		frameBuffer.Create(Window::currentWidth, Window::currentHeight);
		frameBuffer.CreateAttachment();

		// Load shadow map
		g_renderData.shadowMap.Init();

		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");

		// load animations
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

		}

		Player& player = Game::GetPLayerByIndex(0);
		Animator* glockAnimator = AssetManager::GetAnimatorByName("GlockAnimator");
		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = player.camera.getViewMatrix();
		projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);

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
		if (g_renderData.frameBuffers[0].GetWidth() != Window::currentWidth || g_renderData.frameBuffers[0].GetHeight() != Window::currentHeight) {
			g_renderData.frameBuffers[0].Resize(Window::currentWidth, Window::currentHeight);
		}

		g_renderData.frameBuffers[0].Bind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------ RENDER PASS ------
		_shaders.weaponShader.activate();
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
		if (player.GetEquipedWeaponInfo()->name == "Glock") {
			auto transforms = glockAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel(player.GetEquipedWeaponInfo()->name, _shaders.weaponShader);
		}
		else {
			auto transforms = p90Animator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel(player.GetEquipedWeaponInfo()->name, _shaders.weaponShader);
		}

		/*_shaders.animShader.activate();
		_shaders.animShader.setMat4("view", view);
		_shaders.animShader.setMat4("projection", projection);
		_shaders.animShader.setFloat("gamma", g_renderData.gamma);
		auto p90Transforms = p90Animator->GetFinalBoneMatrices();
		for (int i = 0; i < p90Transforms.size(); ++i)
			_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", p90Transforms[i]);
		AssetManager::DrawModel("P90", _shaders.animShader);*/

		_shaders.texturedObjectShader.activate();
		_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
		_shaders.texturedObjectShader.setMat4("view", view);
		_shaders.texturedObjectShader.setMat4("projection", projection);
		_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
		_shaders.texturedObjectShader.setInt("noPointLights", g_renderData.sceneLights.size());
		_shaders.texturedObjectShader.setInt("shadowMap", 3);
		_shaders.texturedObjectShader.setFloat("gamma", g_renderData.gamma);
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
		_shaders.texturedObjectShader.setFloat("gamma", g_renderData.gamma);
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
		/*Scene::GetPrimitiveModelByName("CubeLamp")->draw(_shaders.lampShader);*/
		AssetManager::DrawModel("CubeLamp", _shaders.lampShader);

		// ------ BULLET PASS ----------

		std::vector<glm::vec3> translations;
		float offset = 0.8f;

		for (int y = -10; y < 10; y += 2) {
			for (int x = -10; x < 10; x += 2) {
				translations.emplace_back(x * offset, 10.0f, translations.size() * offset);
			}
		}

		_shaders.instancedShader.activate();
		_shaders.instancedShader.setMat4("view", view);
		_shaders.instancedShader.setMat4("projection", projection);
		for (unsigned int i = 0; i < 10; i++)
		{
			_shaders.instancedShader.setVec3("offsets[" + std::to_string(i) + "]", translations[i]);
		}

		AssetManager::DrawModelInstanced("Bullet", _shaders.instancedShader, 10);

		/*for (int i = 0; i < 5; i++) {
			Model* bulletModel = AssetManager::GetModelByName("Bullet");

			bulletModel->setPosition(glm::vec3(i * 2.0f, 1.0f, 0.0f));

			AssetManager::DrawModel("Bullet", _shaders.testShader);
		}*/

		// ------ CUBEMAP PASS -------------
		g_renderData.cubeMaps[0].render(_shaders.skyboxShader, player.camera.getViewMatrix(), projection);


		// ------ FRAME BUFFER PASS -----------

		/*glActiveTexture(GL_TEXTURE0);*/
		g_renderData.frameBuffers[0].Unbind();

		switch (g_renderData.currentMode)
		{
		case RendererCommon::PostProcessMode::NONE:
			_shaders.postProcessShader.activate();
			_shaders.postProcessShader.setInt("screenTexture", 0);
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
		g_renderData.frameBuffers[0].BindTexture();

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	RendererCommon::PostProcessMode GetPostProcessMode() {
		return g_renderData.currentMode;
	}

	void ChangePostProcessMode(RendererCommon::PostProcessMode mode) {
		g_renderData.currentMode = mode;
	}

	float GetGammaValue() {
		return g_renderData.gamma;
	}
	void ChangeGammaValue(float value) {
		g_renderData.gamma = value;
	}

	void Cleanup() {
		g_renderData.shadowMap.Cleanup();

		for (CubeMap cubeMap : g_renderData.cubeMaps) {
			cubeMap.cleanup();
		};

		for (FrameBuffer frameBuffer : g_renderData.frameBuffers) {
			frameBuffer.Cleanup();
		}
		
	}
}