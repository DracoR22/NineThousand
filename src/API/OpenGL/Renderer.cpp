#include "Renderer.h"

namespace OpenGLRenderer {

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
		Shader blurShader;
		Shader lightingShader;
		Shader muzzleFlashShader;
		Shader waterShader;
		Shader simpleTextureShader;
	} g_shaders;

	//ModelCreateInfo bulletCreateInfo{
	//		glm::vec3(0.0f, 5.0f, 1.0f),
	//		glm::vec3(0.75f),
	//		glm::mat4(1.0f),
	//		{                                     // instanceOffsets (hardcoded)
	//	glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(2.0f, 0.0f, 0.0f),
	//	glm::vec3(-2.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 2.0f, 0.0f),
	//	glm::vec3(0.0f, -2.0f, 0.0f)
	//}
	//};

	struct RenderData {
		unsigned int frameBufferQuadVAO = 0;
		unsigned int frameBufferQuadVBO = 0;

		std::vector<LightCreateInfo> sceneLights = {};
		std::vector<CubeMap> cubeMaps = {};

		ShadowMap shadowMap;

		Mesh2D textMesh;
		Mesh2D crossHairMesh;
		Mesh2D muzzleFlashMesh;

		RendererCommon::PostProcessMode currentMode = RendererCommon::PostProcessMode::NONE;

		float gamma = 2.2f;
		float exposure = 1.0f;
	} g_renderData;

	struct RenderFrameBuffer {
		FrameBuffer postProcessingFrameBuffer;
		FrameBuffer mssaFrameBuffer;
		FrameBuffer refractionFrameBuffer;

		/*std::vector<FrameBuffer> pingPongFrameBuffers;*/
	} g_renderFrameBuffers;

	void Init() {
		// load shaders
		g_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
		g_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
		g_shaders.animShader.load("animated.vert", "animated.frag");
		g_shaders.lampShader.load("lamp.vert", "lamp.frag");
		g_shaders.shadowMapShader.load("shadow_map.vert", "shadow_map.frag");
		g_shaders.weaponShader.load("weapon.vert", "weapon.frag");
		g_shaders.sharpenShader.load("sharpen.vert", "sharpen.frag");
		g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
		g_shaders.testShader.load("test.vert", "test.frag");
		g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
		g_shaders.uiShader.load("ui.vert", "ui.frag");
		g_shaders.blurShader.load("blur.vert", "blur.frag");
		g_shaders.lightingShader.load("lighting.vert", "lighting.frag");
		g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
		g_shaders.waterShader.load("water.vert", "water.frag");
		g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");

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

		// load 2d meshes
		g_renderData.textMesh.Create();
		g_renderData.crossHairMesh.Create();
		g_renderData.muzzleFlashMesh.Create();

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
		 glm::vec3(0.0f, 2.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo cubeCreateInfo{
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f),
			"PoolTile_ALB.png",
			"PoolTile_NRM.png",
			"PoolTile_RMA.png"
		};

		ModelCreateInfo lampCreateInfo{
			glm::vec3(10.0f, 5.0f, 5.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo planeCreateInfo{
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::mat4(1.0f),
			"PoolTile_ALB.png",
			"PoolTile_NRM.png",
			"PoolTile_RMA.png"
		};

		ModelCreateInfo waterPlaneCreateInfo{
			glm::vec3(0.0f, 0.5f, 0.0f),
			glm::vec3(20.0f),
			glm::mat4(1.0f),
			"WaterDUDV.png",
			"WaterNormal.png",
		};

		AssetManager::LoadAssimpModel("P90", "resources/models/P90T.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);
		AssetManager::LoadAssimpModel("AKS74U", "resources/models/AKS74UBfbx.fbx", aks74uCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);
		AssetManager::LoadModel("WaterPlane", ModelType::PLANE, waterPlaneCreateInfo);
		/*AssetManager::LoadModel("Bullet", ModelType::CUBE, bulletCreateInfo);*/

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
		cubeLampLight.position = AssetManager::GetModelByName("CubeLamp")->pos;
		cubeLampLight.constant = 1.0f;
		cubeLampLight.linear = 0.0014f;
		cubeLampLight.quadratic = 0.000007f;
		cubeLampLight.ambient = glm::vec3(0.002f);
		cubeLampLight.diffuse = glm::vec3(0.8f);
		cubeLampLight.specular = glm::vec3(1.0f);
		cubeLampLight.color = glm::vec3(0.8f);
		cubeLampLight.radius = 70.0f;
		cubeLampLight.strength = 1.0f;
		cubeLampLight.type = LightType::POINT_LIGHT;

		LightCreateInfo cubeLampLight2;
		cubeLampLight2.position = AssetManager::GetModelByName("Cube")->pos;
		cubeLampLight2.constant = 1.0f;
		cubeLampLight2.linear = 0.0014f;
		cubeLampLight2.quadratic = 0.000007f;
		cubeLampLight2.ambient = glm::vec3(0.002f);
		cubeLampLight2.diffuse = glm::vec3(0.8f);
		cubeLampLight2.specular = glm::vec3(1.0f);
		cubeLampLight2.color = glm::vec3(0.8f);
		cubeLampLight2.radius = 70.0f;
		cubeLampLight2.strength = 1.0f;
		cubeLampLight2.type = LightType::POINT_LIGHT;

		g_renderData.sceneLights.push_back(cubeLampLight);
		g_renderData.sceneLights.push_back(cubeLampLight2);

		// Load frame buffers
		g_renderFrameBuffers.postProcessingFrameBuffer.Create(Window::currentWidth, Window::currentHeight);
		g_renderFrameBuffers.postProcessingFrameBuffer.Bind();
		g_renderFrameBuffers.postProcessingFrameBuffer.CreateAttachment("hdrAttachment");
		g_renderFrameBuffers.postProcessingFrameBuffer.CreateDepthAttachment();
		g_renderFrameBuffers.postProcessingFrameBuffer.DrawBuffer();
		g_renderFrameBuffers.postProcessingFrameBuffer.Unbind();

		g_renderFrameBuffers.refractionFrameBuffer.Create(Window::currentWidth, Window::currentHeight);
		g_renderFrameBuffers.refractionFrameBuffer.Bind();
		g_renderFrameBuffers.refractionFrameBuffer.CreateAttachment("refractionAttachment");
		g_renderFrameBuffers.refractionFrameBuffer.CreateDepthTextureAttachment();
		g_renderFrameBuffers.refractionFrameBuffer.DrawBuffer();
		g_renderFrameBuffers.refractionFrameBuffer.Unbind();

		g_renderFrameBuffers.mssaFrameBuffer.Create(Window::currentWidth, Window::currentHeight);
		g_renderFrameBuffers.mssaFrameBuffer.Bind();
		g_renderFrameBuffers.mssaFrameBuffer.CreateMSAAAttachment("msaaAttachment");
		g_renderFrameBuffers.mssaFrameBuffer.DrawBuffer();
		g_renderFrameBuffers.mssaFrameBuffer.Unbind();


		/*g_renderFrameBuffers.pingPongFrameBuffers.resize(2);
		for (int i = 0; i < 2; i++) {
			g_renderFrameBuffers.pingPongFrameBuffers[i].Create(Window::currentWidth, Window::currentHeight);
			g_renderFrameBuffers.pingPongFrameBuffers[i].Bind();
			g_renderFrameBuffers.pingPongFrameBuffers[i].CreateAttachment(("pingPong" + std::to_string(i)).c_str());
			g_renderFrameBuffers.pingPongFrameBuffers[i].DrawBuffer();
			g_renderFrameBuffers.pingPongFrameBuffers[i].Unbind();
		}*/

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
		AssetManager::LoadAnimation("AKS74U_ADS_In", "resources/animations/AKS74U_ADS_In.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_ADS_Out", "resources/animations/AKS74U_ADS_Out.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_ADS_Idle", "resources/animations/AKS74U_ADS_Idle.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_ADS_Fire", "resources/animations/AKS74U_ADS_Fire0.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_ADS_Walk", "resources/animations/AKS74U_ADS_Walk.fbx", aks74uModel);

		AssetManager::LoadAnimation("Glock_Idle", "resources/animations/Glock_Idle.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Reload", "resources/animations/Glock_Reload.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ReloadEmpty", "resources/animations/Glock_ReloadEmpty.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Walk", "resources/animations/Glock_Walk.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Fire0", "resources/animations/Glock_Fire0.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Draw", "resources/animations/Glock_Draw.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ADS_In", "resources/animations/Glock_ADS_In.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ADS_Out", "resources/animations/Glock_ADS_Out.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ADS_Fire", "resources/animations/Glock_ADS_Fire1.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ADS_Idle", "resources/animations/Glock_ADS_Idle.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_ADS_Walk", "resources/animations/Glock_ADS_Walk.fbx", glockModel);

		AssetManager::LoadAnimation("P90_Idle", "resources/animations/P90_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Reload", "resources/animations/P90_Reload.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Walk", "resources/animations/P90_Walk.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Fire0", "resources/animations/P90_Fire0.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Draw", "resources/animations/P90_Draw.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_In", "resources/animations/P90_ADS_In.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Out", "resources/animations/P90_ADS_Out.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Idle", "resources/animations/P90_ADS_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Fire", "resources/animations/P90_ADS_Fire0.fbx", p90Model);

		AssetManager::LoadAnimator("GlockAnimator", AssetManager::GetAnimationByName("Glock_Idle"));
		AssetManager::LoadAnimator("P90Animator", AssetManager::GetAnimationByName("P90_Idle"));
		AssetManager::LoadAnimator("AKS74UAnimator", AssetManager::GetAnimationByName("AKS74U_Idle"));
	}

	void RenderFrame() {
		// Hotload shaders
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			g_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
			g_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
			g_shaders.animShader.load("animated.vert", "animated.frag");
			g_shaders.lampShader.load("lamp.vert", "lamp.frag");
			g_shaders.weaponShader.load("weapon.vert", "weapon.frag");
			g_shaders.sharpenShader.load("sharpen.vert", "sharpen.frag");
			g_shaders.testShader.load("test.vert", "test.frag");
			g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
			g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
			g_shaders.blurShader.load("blur.vert", "blur.frag");
			g_shaders.lightingShader.load("lighting.vert", "lighting.frag");
			g_shaders.uiShader.load("ui.vert", "ui.frag");
			g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
			g_shaders.waterShader.load("water.vert", "water.frag");
			g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");
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
		glm::mat4 lightView = glm::mat4(1.0f);

		if (g_renderData.sceneLights.empty()) {
			lightView = glm::mat4(1.0f);
		}
		else {
			lightView = glm::lookAt(g_renderData.sceneLights[0].position, AssetManager::GetModelByName("Cube")->pos, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glm::mat4 lightProjection = orthogonalProjection * lightView;

		g_renderData.shadowMap.Clear();

		glCullFace(GL_FRONT);

		g_shaders.shadowMapShader.activate();
		g_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);
		AssetManager::DrawModel("Cube", g_shaders.shadowMapShader);

		glCullFace(GL_BACK);

		g_renderData.shadowMap.Unbind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*glEnable(GL_CLIP_DISTANCE0);*/
		// REFLECTION PASS 
		g_renderFrameBuffers.refractionFrameBuffer.Bind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_shaders.simpleTextureShader.activate();
		g_shaders.simpleTextureShader.setMat4("view", view);
		g_shaders.simpleTextureShader.setMat4("projection", projection);
		/*g_shaders.lightingShader.setMat4("lightProjection", lightProjection);
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			g_shaders.lightingShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			g_shaders.lightingShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			g_shaders.lightingShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			g_shaders.lightingShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);
			g_shaders.lightingShader.setFloat(lightUniform + ".radius", g_renderData.sceneLights[i].radius);
			g_shaders.lightingShader.setFloat(lightUniform + ".strength", g_renderData.sceneLights[i].strength);

			g_shaders.lightingShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			g_shaders.lightingShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			g_shaders.lightingShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
			g_shaders.lightingShader.setVec3(lightUniform + ".color", g_renderData.sceneLights[i].color);
			g_shaders.lightingShader.setInt(lightUniform + ".type", static_cast<int>(g_renderData.sceneLights[i].type));
		}
		g_shaders.lightingShader.setInt("noLights", g_renderData.sceneLights.size());
		g_shaders.lightingShader.set3Float("camPos", player.getPosition());
		g_shaders.lightingShader.setInt("shadowMap", 3);*/
		AssetManager::DrawModel("Plane", g_shaders.simpleTextureShader);
		// AssetManager::DrawModel("Cube", g_shaders.lightingShader);

		//g_renderData.cubeMaps[0].render(g_shaders.skyboxShader, view, projection);

		g_renderFrameBuffers.refractionFrameBuffer.Unbind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------ BIND FRAME BUFFERS -------
		if (g_renderFrameBuffers.postProcessingFrameBuffer.GetWidth() != Window::currentWidth || g_renderFrameBuffers.postProcessingFrameBuffer.GetHeight() != Window::currentHeight) {
			g_renderFrameBuffers.postProcessingFrameBuffer.Resize(Window::currentWidth, Window::currentHeight);
			g_renderFrameBuffers.refractionFrameBuffer.Resize(Window::currentWidth, Window::currentHeight);
			g_renderFrameBuffers.mssaFrameBuffer.ResizeMSAA(Window::currentWidth, Window::currentHeight);
		}

		g_renderFrameBuffers.mssaFrameBuffer.Bind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------ RENDER PASS ------
		g_shaders.weaponShader.activate();
		g_shaders.weaponShader.setMat4("view", view);
		g_shaders.weaponShader.setMat4("projection", projection);
		g_shaders.weaponShader.setMat4("lightProjection", lightProjection);
		g_shaders.weaponShader.setInt("noLights", g_renderData.sceneLights.size());
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			g_shaders.weaponShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			g_shaders.weaponShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			g_shaders.weaponShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			g_shaders.weaponShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);
			g_shaders.weaponShader.setFloat(lightUniform + ".radius", g_renderData.sceneLights[i].radius);
			g_shaders.weaponShader.setFloat(lightUniform + ".strength", g_renderData.sceneLights[i].strength);

			g_shaders.weaponShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			g_shaders.weaponShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			g_shaders.weaponShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
			g_shaders.weaponShader.setVec3(lightUniform + ".color", g_renderData.sceneLights[i].color);
			g_shaders.weaponShader.setInt(lightUniform + ".type", static_cast<int>(g_renderData.sceneLights[i].type));
		}
		g_shaders.weaponShader.set3Float("camPos", player.getPosition());
		if (player.PressingADS() || player.GetWeaponAction() == WeaponAction::ADS_OUT) { // little hack because ads light was getting rotated
			g_shaders.weaponShader.setBool("flipLights", true);
		}
		else {
			g_shaders.weaponShader.setBool("flipLights", false);
		}
		if (player.GetEquipedWeaponInfo()->name == "Glock") {
			auto& transforms = glockAnimator->GetFinalBoneMatrices();
			
			for (int i = 0; i < transforms.size(); ++i) {
				g_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}

			AssetManager::DrawModel("Glock", g_shaders.weaponShader);
		}
		else if (player.GetEquipedWeaponInfo()->name == "AKS74U") {
			auto transforms = aks74uAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				g_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}

			AssetManager::DrawModel("AKS74U", g_shaders.weaponShader);
		}	
		else if (player.GetEquipedWeaponInfo()->name == "P90") {
			auto transforms = p90Animator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				g_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("P90", g_shaders.weaponShader);
		}

		// TO TEST PBR WORKS CORRECTLY FOR NOW-------------------------------------------------------
		g_shaders.lightingShader.activate();
		g_shaders.lightingShader.setMat4("view", view);
		g_shaders.lightingShader.setMat4("projection", projection);
		g_shaders.lightingShader.setMat4("lightProjection", lightProjection);
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			g_shaders.lightingShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			g_shaders.lightingShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			g_shaders.lightingShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			g_shaders.lightingShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);
			g_shaders.lightingShader.setFloat(lightUniform + ".radius", g_renderData.sceneLights[i].radius);
			g_shaders.lightingShader.setFloat(lightUniform + ".strength", g_renderData.sceneLights[i].strength);

			g_shaders.lightingShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			g_shaders.lightingShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			g_shaders.lightingShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
			g_shaders.lightingShader.setVec3(lightUniform + ".color", g_renderData.sceneLights[i].color);
			g_shaders.lightingShader.setInt(lightUniform + ".type", static_cast<int>(g_renderData.sceneLights[i].type));
		}
		g_shaders.lightingShader.setInt("noLights", g_renderData.sceneLights.size());
		g_shaders.lightingShader.set3Float("camPos", player.getPosition());
		g_shaders.lightingShader.setInt("shadowMap", 3);
		AssetManager::DrawModel("Plane", g_shaders.lightingShader);
		AssetManager::DrawModel("Cube", g_shaders.lightingShader);

		// WATER PASS
		static float moveFactor = 0.0f;
		float waveSpeed = 0.03f;
		moveFactor += waveSpeed * Window::GetDeltaTime();
		if (moveFactor > 1.0f)
			moveFactor -= 1.0f;

		g_shaders.waterShader.activate();
		g_shaders.waterShader.setMat4("view", view);
		g_shaders.waterShader.setMat4("projection", projection);
		g_shaders.waterShader.setVec3("camPos", player.getPosition());

		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			g_shaders.waterShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			g_shaders.waterShader.setVec3(lightUniform + ".color", g_renderData.sceneLights[i].color);
			g_shaders.waterShader.setFloat(lightUniform + ".radius", g_renderData.sceneLights[i].radius);
		}
		g_shaders.waterShader.setInt("lightsNr", g_renderData.sceneLights.size());

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, g_renderFrameBuffers.refractionFrameBuffer.GetColorAttachmentTextureIdByIndex(0));
		g_shaders.waterShader.setInt("refractionColor", 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, g_renderFrameBuffers.refractionFrameBuffer.GetDepthTextureAttachmentId());
		g_shaders.waterShader.setInt("depthTexture", 5);

		g_shaders.waterShader.setFloat("moveFactor", moveFactor);

		AssetManager::DrawModel("WaterPlane", g_shaders.waterShader);

		// DEBUG LIGHTS
		g_shaders.lampShader.activate();
		g_shaders.lampShader.set3Float("viewPos", player.getPosition());
		g_shaders.lampShader.setMat4("view", view);
		g_shaders.lampShader.setMat4("projection", projection);
		g_shaders.lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		AssetManager::DrawModel("CubeLamp", g_shaders.lampShader);

		// ------ INSTANCE PASS ----------
		/*g_shaders.instancedShader.activate();
		g_shaders.instancedShader.setMat4("view", view);
		g_shaders.instancedShader.setMat4("projection", projection);*/

		//AssetManager::DrawModelInstanced("Bullet", g_shaders.instancedShader, bulletCreateInfo.instanceOffsets);	

		// ------ CUBEMAP PASS -------------
		g_renderData.cubeMaps[0].render(g_shaders.skyboxShader, player.camera.getViewMatrix(), projection);

		Model* weaponModel = AssetManager::GetModelByName(player.GetEquipedWeaponInfo()->name);

		// ------ MUZZLE FLASH PASS
		glm::vec3 barrelOffset = player.GetEquipedWeaponInfo()->muzzleFlashOffset;
		glm::mat4 gunTransform = glm::translate(glm::mat4(1.0f), weaponModel->pos) * weaponModel->rotation;

		glm::vec4 worldBarrelPos = gunTransform * glm::vec4(barrelOffset, 1.0f);

		glm::mat4 muzzleFlashModel = glm::mat4(1.0f);

		muzzleFlashModel = glm::translate(muzzleFlashModel, glm::vec3(worldBarrelPos));

	    muzzleFlashModel *= glm::inverse(glm::mat4(glm::mat3(view)));
		muzzleFlashModel = glm::scale(muzzleFlashModel, glm::vec3(4.0f));

		float randomAngle = Utils::RandomFloat(0.0f, glm::two_pi<float>());

		g_shaders.muzzleFlashShader.activate();

		g_shaders.muzzleFlashShader.setMat4("model", muzzleFlashModel);
		g_shaders.muzzleFlashShader.setMat4("view", view);
		g_shaders.muzzleFlashShader.setMat4("projection", projection);
		g_shaders.muzzleFlashShader.setFloat("rotation", randomAngle);

		Texture* muzzleFlashTexture = AssetManager::GetTextureByName("MuzzleFlash.png");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, muzzleFlashTexture->id);

		if (player._muzzleFlashTimer > 0) {
			g_renderData.muzzleFlashMesh.RenderTexture(g_shaders.muzzleFlashShader);
			player._muzzleFlashTimer--;
		}

		// ------ UI PASS -------------
		glDisable(GL_DEPTH_TEST);
		glm::mat4 UiProjection = glm::ortho(0.0f, (float)Window::currentWidth, (float)Window::currentHeight, 0.0f);

		UIElement fpsTextElement;
		fpsTextElement.m_alignment = UIAlignment::TopLeft;
		fpsTextElement.m_size = glm::vec2(0.0f, 30.0f);

		g_shaders.uiShader.activate();
		g_shaders.uiShader.setMat4("projection", UiProjection);
	
		float fpsTextX = 10.0f;  // offset from the left
		float fpsTextY = 30.0f;
		float debugFontSize = 0.45f;  

		float posTextX = 10.0f;
		float posTextY = 45.0f;

		glm::mat4 uiModel = glm::mat4(1.0f);
		uiModel = glm::translate(uiModel, glm::vec3(fpsTextX, fpsTextY, 0.0f));
		uiModel = glm::scale(uiModel, glm::vec3(debugFontSize, debugFontSize, 1.0f));
		g_shaders.uiShader.setMat4("model", uiModel);

		Texture* sansFontTexture = AssetManager::GetTextureByName("sans.png");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sansFontTexture->id);

		g_renderData.textMesh.RenderText("FPS: " + std::to_string(Window::GetFPSCount()), fpsTextX, fpsTextY, debugFontSize, glm::vec3(1.0f, 1.0f, 1.0f), g_shaders.uiShader);

		std::string playerPosText = "Player Position: (" +
			std::to_string(player.getPosition().x) + ", " +
			std::to_string(player.getPosition().y) + ", " +
			std::to_string(player.getPosition().z) + ")";

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sansFontTexture->id);

		g_shaders.uiShader.activate();
		g_shaders.uiShader.setMat4("projection", UiProjection);
		glm::mat4 posModel = glm::mat4(1.0f);
		posModel = glm::translate(posModel, glm::vec3(posTextX, posTextY, 0.0f));
		posModel = glm::scale(posModel, glm::vec3(debugFontSize, debugFontSize, 1.0f));
		g_shaders.uiShader.setMat4("model", posModel);

		g_renderData.textMesh.RenderText(playerPosText, posTextX, posTextY, debugFontSize, glm::vec3(1.0f, 1.0f, 1.0f), g_shaders.uiShader);

		// crosshair
		UIElement crosshairElement;
		crosshairElement.m_size = glm::vec2(50.0f, 50.0f);
		crosshairElement.m_alignment = UIAlignment::Center;

		g_shaders.uiShader.activate();
		g_shaders.uiShader.setMat4("projection", UiProjection);
		g_shaders.uiShader.setMat4("model", crosshairElement.GetModelMatrix());
		g_shaders.uiShader.setInt("baseTexture", 0);

		Texture* crosshairTexture = AssetManager::GetTextureByName("CrossHairDotOutline.png");

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, crosshairTexture->id);
		g_renderData.crossHairMesh.RenderTexture(g_shaders.uiShader);


		glEnable(GL_DEPTH_TEST);


		// ------ POST PROCESS PASS -----------
		glBindFramebuffer(GL_READ_FRAMEBUFFER, g_renderFrameBuffers.mssaFrameBuffer.GetFBO());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_renderFrameBuffers.postProcessingFrameBuffer.GetFBO());
		if (g_renderFrameBuffers.mssaFrameBuffer.GetWidth() == Window::currentWidth && g_renderFrameBuffers.postProcessingFrameBuffer.GetWidth() == Window::currentWidth) {
			glBlitFramebuffer(0, 0, Window::currentWidth, Window::currentHeight, 0, 0, Window::currentWidth, Window::currentHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*GLuint bloomTexture = g_renderFrameBuffers.postProcessingFrameBuffer.GetColorAttachmentTextureIdByIndex(1);*/
	/*	bool horizontal = true;
		int blurIterations = 10;

		_shaders.blurShader.activate(); 
		_shaders.blurShader.setInt("image", 0);
		glActiveTexture(GL_TEXTURE0);
		for (int i = 0; i < blurIterations; i++) {
			g_renderFrameBuffers.pingPongFrameBuffers[horizontal].Bind();
			
			
			_shaders.blurShader.setBool("horizontal", horizontal);

			glBindTexture(GL_TEXTURE_2D, i == 0 ? bloomTexture : g_renderFrameBuffers.pingPongFrameBuffers[!horizontal].GetColorAttachmentTextureIdByIndex(0));

			glBindVertexArray(g_renderData.frameBufferQuadVAO);
			glDisable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			g_renderFrameBuffers.pingPongFrameBuffers[horizontal].Unbind();

			horizontal = !horizontal;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

		switch (g_renderData.currentMode)
		{
		case RendererCommon::PostProcessMode::NONE:
			g_shaders.postProcessShader.activate();
			g_shaders.postProcessShader.setInt("screenTexture", 0);
			g_shaders.postProcessShader.setInt("bloomTexture", 1);
			g_shaders.postProcessShader.setFloat("gamma", g_renderData.gamma);
			g_shaders.postProcessShader.setFloat("exposure", g_renderData.exposure);

			break;
		case RendererCommon::PostProcessMode::SHARPEN:
			g_shaders.sharpenShader.activate();
			g_shaders.sharpenShader.setInt("screenTexture", 0);
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
		glBindTexture(GL_TEXTURE_2D, g_renderFrameBuffers.postProcessingFrameBuffer.GetColorAttachmentTextureIdByIndex(0));
		//*glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, g_renderFrameBuffers.pingPongFrameBuffers[!horizontal].GetColorAttachmentTextureIdByIndex(0));*/
		//*g_renderFrameBuffers.postProcessingFrameBuffer.BindTextures();*/

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

	void Cleanup() {
		g_renderData.shadowMap.Cleanup();
		g_renderData.textMesh.Cleanup();

		for (CubeMap cubeMap : g_renderData.cubeMaps) {
			cubeMap.cleanup();
		};

		g_renderFrameBuffers.postProcessingFrameBuffer.Cleanup();
		g_renderFrameBuffers.refractionFrameBuffer.Cleanup();
		g_renderFrameBuffers.mssaFrameBuffer.Cleanup();
	}
}