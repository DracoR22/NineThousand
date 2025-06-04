#include "Renderer.h"

namespace OpenGLRenderer {

	struct Shaders {
		Shader skyboxShader;
		Shader animationShader;
		Shader shadowMapShader;
		Shader sobelEdgesShader;
		Shader postProcessShader;
		Shader solidColorShader;
		Shader instancedShader;
		Shader uiShader;
		Shader blurShader;
		Shader lightingShader;
		Shader muzzleFlashShader;
		Shader waterShader;
		Shader simpleTextureShader;
		Shader outlineAnimatedShader;
	} g_shaders;

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
		float exposure = 1.0f;;
	} g_renderData;

	struct RenderFrameBuffer {
		FrameBuffer postProcessingFrameBuffer;
		FrameBuffer mssaFrameBuffer;
		FrameBuffer refractionFrameBuffer;

		/*std::vector<FrameBuffer> pingPongFrameBuffers;*/
	} g_renderFrameBuffers;

	void Init() {
		Player& player = Game::GetPLayerByIndex(0);

		// load shaders
		g_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
		g_shaders.animationShader.load("animated.vert", "animated.frag");
		g_shaders.shadowMapShader.load("shadow_map.vert", "shadow_map.frag");
		g_shaders.sobelEdgesShader.load("sobel_edges.vert", "sobel_edges.frag");
		g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
		g_shaders.solidColorShader.load("solid_color.vert", "solid_color.frag");
		g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
		g_shaders.uiShader.load("ui.vert", "ui.frag");
		g_shaders.blurShader.load("blur.vert", "blur.frag");
		g_shaders.lightingShader.load("lighting.vert", "lighting.frag");
		g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
		g_shaders.waterShader.load("water.vert", "water.frag");
		g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");
		g_shaders.outlineAnimatedShader.load("outline_animated.vert", "outline_animated.frag");

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
		cubeLampLight2.position = glm::vec3(7.0f, 5.0f, 2.0f);
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
	}

	void RenderFrame() {
		// Hotload shaders
		if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
			g_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
			g_shaders.animationShader.load("animated.vert", "animated.frag");
			g_shaders.sobelEdgesShader.load("sobel_edges.vert", "sobel_edges.frag");
			g_shaders.solidColorShader.load("solid_color.vert", "solid_color.frag");
			g_shaders.instancedShader.load("instanced.vert", "instanced.frag");
			g_shaders.postProcessShader.load("post_process.vert", "post_process.frag");
			g_shaders.blurShader.load("blur.vert", "blur.frag");
			g_shaders.lightingShader.load("lighting.vert", "lighting.frag");
			g_shaders.uiShader.load("ui.vert", "ui.frag");
			g_shaders.muzzleFlashShader.load("muzzle_flash.vert", "muzzle_flash.frag");
			g_shaders.waterShader.load("water.vert", "water.frag");
			g_shaders.simpleTextureShader.load("simple_texture.vert", "simple_texture.frag");
			g_shaders.outlineAnimatedShader.load("outline_animated.vert", "outline_animated.frag");
		}

		Player& player = Game::GetPLayerByIndex(0);

		Animator* dEagleAnimator = AssetManager::GetAnimatorByName("DEAGLEAnimator");

		dEagleAnimator->UpdateAnimation(Window::GetDeltaTime());
		if (Keyboard::KeyJustPressed(GLFW_KEY_V)) {
			dEagleAnimator->PlayAnimation(AssetManager::GetAnimationByName("DEAGLE_Walk"));
		}

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = CameraManager::GetActiveCamera()->getViewMatrix();
		projection = glm::perspective(glm::radians(CameraManager::GetActiveCamera()->getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 500.0f);

		// ------ SHADOW PASS --------
		glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		glm::mat4 lightView = glm::mat4(1.0f);

		if (g_renderData.sceneLights.empty()) {
			lightView = glm::mat4(1.0f);
		}
		else {
			lightView = glm::lookAt(g_renderData.sceneLights[0].position, Scene::GetGameObjectByName("Cube0")->GetPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		glm::mat4 lightProjection = orthogonalProjection * lightView;

		g_renderData.shadowMap.Clear();

		glCullFace(GL_FRONT);

		g_shaders.shadowMapShader.activate();
		g_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);

		glm::mat4 smodel = glm::mat4(1.0f);
		smodel = glm::translate(smodel, Scene::GetGameObjectByName("Cube0")->GetPosition());
		smodel = glm::scale(smodel, Scene::GetGameObjectByName("Cube0")->GetSize());
		smodel *= Scene::GetGameObjectByName("Cube0")->GetRotationMatrix();
		g_shaders.shadowMapShader.setMat4("model", smodel);
		AssetManager::DrawModel("Cube", g_shaders.shadowMapShader);

		glCullFace(GL_BACK);

		g_renderData.shadowMap.Unbind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
		// REFLECTION PASS 
		g_renderFrameBuffers.refractionFrameBuffer.Bind();
		glViewport(0, 0, Window::currentWidth, Window::currentHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		g_shaders.simpleTextureShader.activate();
		g_shaders.simpleTextureShader.setMat4("view", view);
		g_shaders.simpleTextureShader.setMat4("projection", projection);
	
		glm::mat4 rmodel = glm::mat4(1.0f);
		rmodel = glm::translate(rmodel, Scene::GetGameObjectByName("Plane0")->GetPosition());
		rmodel = glm::scale(rmodel, Scene::GetGameObjectByName("Plane0")->GetSize());
		rmodel *= Scene::GetGameObjectByName("Plane0")->GetRotationMatrix();
		g_shaders.simpleTextureShader.setMat4("model", rmodel);
		AssetManager::DrawModel("Plane", g_shaders.simpleTextureShader);

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

		// STENCIL STUFF
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// ------ RENDER PASS ------

		// ANIMATION PASS
		g_shaders.animationShader.activate();
		g_shaders.animationShader.setMat4("view", view);
		g_shaders.animationShader.setMat4("projection", projection);
		g_shaders.animationShader.setMat4("lightProjection", lightProjection);
		g_shaders.animationShader.setInt("noLights", g_renderData.sceneLights.size());
		for (int i = 0; i < g_renderData.sceneLights.size(); i++) {
			std::string lightUniform = "lights[" + std::to_string(i) + "]";

			g_shaders.animationShader.setVec3(lightUniform + ".position", g_renderData.sceneLights[i].position);
			g_shaders.animationShader.setFloat(lightUniform + ".constant", g_renderData.sceneLights[i].constant);
			g_shaders.animationShader.setFloat(lightUniform + ".linear", g_renderData.sceneLights[i].linear);
			g_shaders.animationShader.setFloat(lightUniform + ".quadratic", g_renderData.sceneLights[i].quadratic);
			g_shaders.animationShader.setFloat(lightUniform + ".radius", g_renderData.sceneLights[i].radius);
			g_shaders.animationShader.setFloat(lightUniform + ".strength", g_renderData.sceneLights[i].strength);

			g_shaders.animationShader.setVec3(lightUniform + ".ambient", g_renderData.sceneLights[i].ambient);
			g_shaders.animationShader.setVec3(lightUniform + ".diffuse", g_renderData.sceneLights[i].diffuse);
			g_shaders.animationShader.setVec3(lightUniform + ".specular", g_renderData.sceneLights[i].specular);
			g_shaders.animationShader.setVec3(lightUniform + ".color", g_renderData.sceneLights[i].color);
			g_shaders.animationShader.setInt(lightUniform + ".type", static_cast<int>(g_renderData.sceneLights[i].type));
		}
		g_shaders.animationShader.set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);
		//if (player.PressingADS() || player.GetWeaponAction() == WeaponAction::ADS_OUT) { // little hack because ads light was getting rotated
		//	g_shaders.weaponShader.setBool("flipLights", true);
		//}
		//else {
		//	g_shaders.weaponShader.setBool("flipLights", false);
		//}

		glm::mat4 amodel = glm::mat4(1.0f);
		amodel = glm::translate(amodel, player.m_currentWeaponGameObject.GetPosition());
		amodel = glm::scale(amodel, player.m_currentWeaponGameObject.GetSize());
		amodel *= player.m_currentWeaponGameObject.GetRotationMatrix();
		g_shaders.animationShader.setMat4("model", amodel);

		auto& transforms = AssetManager::GetAnimatorByName(player.GetEquipedWeaponInfo()->name + "Animator")->GetFinalBoneMatrices();
		for (int i = 0; i < transforms.size(); ++i) {
			g_shaders.animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		}
		AssetManager::DrawModel(player.GetEquipedWeaponInfo()->name, g_shaders.animationShader);

		glm::mat4 bmodel = glm::mat4(1.0f);
		bmodel = glm::translate(bmodel, glm::vec3(0.0f, 2.0f, 0.0f));
		bmodel = glm::scale(bmodel, glm::vec3(0.05f));
		bmodel *= glm::mat4(1.0f);
		g_shaders.animationShader.setMat4("model", bmodel);
		
		auto& etransforms = dEagleAnimator->GetFinalBoneMatrices();
		for (int i = 0; i < etransforms.size(); ++i) {
			g_shaders.animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", etransforms[i]);
		}
		AssetManager::DrawModel("DEAGLE", g_shaders.animationShader);

		// LIGHTING PASS
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
		g_shaders.lightingShader.set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);
		g_shaders.lightingShader.setInt("shadowMap", 3);

		for (GameObject& gameObject : Scene::GetGameObjects()) {
			glm::mat4 cmodel = glm::mat4(1.0f);

			cmodel = glm::translate(cmodel, gameObject.GetPosition());
			cmodel = glm::scale(cmodel, gameObject.GetSize());
			cmodel *= gameObject.GetRotationMatrix();

			g_shaders.lightingShader.setMat4("model", cmodel);
			g_shaders.lightingShader.setFloat("textureScale", gameObject.GetTextureScale());

			if (gameObject.IsSelected()) {
				glStencilFunc(GL_ALWAYS, 1, 0xFF); 
				glStencilMask(0xFF); 
			}
			else {
				glStencilMask(0x00);
			}

			AssetManager::DrawModel(gameObject.GetModelName(), g_shaders.lightingShader);
		}
		glStencilMask(0x00);
		

		//AssetManager::DrawModel("Plane", g_shaders.lightingShader);
		//AssetManager::DrawModel("Cube", g_shaders.lightingShader);

		// WATER PASS
		static float moveFactor = 0.0f;
		float waveSpeed = 0.03f;
		moveFactor += waveSpeed * Window::GetDeltaTime();
		if (moveFactor > 1.0f)
			moveFactor -= 1.0f;

		g_shaders.waterShader.activate();
		g_shaders.waterShader.setMat4("view", view);
		g_shaders.waterShader.setMat4("projection", projection);
		g_shaders.waterShader.setVec3("camPos", CameraManager::GetActiveCamera()->cameraPos);

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

		for (WaterPlaneObject& waterPlaneObject: Scene::GetWaterPlaneObjects()) {
			glm::mat4 lmodel = glm::mat4(1.0f);
			lmodel = glm::translate(lmodel, waterPlaneObject.GetPosition());
			lmodel = glm::scale(lmodel, waterPlaneObject.GetSize());
			lmodel *= waterPlaneObject.GetRotation();
			g_shaders.waterShader.setMat4("model", lmodel);
			AssetManager::DrawModel("WaterPlane", g_shaders.waterShader);
		}

		// DEBUG LIGHTS
		g_shaders.solidColorShader.activate();
		g_shaders.solidColorShader.set3Float("viewPos", CameraManager::GetActiveCamera()->cameraPos);
		g_shaders.solidColorShader.setMat4("view", view);
		g_shaders.solidColorShader.setMat4("projection", projection);
		g_shaders.solidColorShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		glm::mat4 lmodel = glm::mat4(1.0f);
		lmodel = glm::translate(lmodel, glm::vec3(10.0f, 5.0f, 5.0f));
		lmodel = glm::scale(lmodel, glm::vec3(0.75f));
		lmodel *= glm::mat4(1.0f);
		g_shaders.solidColorShader.setMat4("model", lmodel);
		AssetManager::DrawModel("CubeLamp", g_shaders.solidColorShader);

		// DEBUG PASS


		// ------ INSTANCE PASS ----------
		/*g_shaders.instancedShader.activate();
		g_shaders.instancedShader.setMat4("view", view);
		g_shaders.instancedShader.setMat4("projection", projection);*/

		//AssetManager::DrawModelInstanced("Bullet", g_shaders.instancedShader, bulletCreateInfo.instanceOffsets);	

		// ------ CUBEMAP PASS -------------
		g_renderData.cubeMaps[0].Draw(g_shaders.skyboxShader, CameraManager::GetActiveCamera()->getViewMatrix(), projection);

		// OUTLINE PASS
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		g_shaders.outlineAnimatedShader.activate();
		g_shaders.outlineAnimatedShader.setMat4("view", view);
		g_shaders.outlineAnimatedShader.setMat4("projection", projection);
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
		
		// ------ MUZZLE FLASH PASS
		glm::vec3 barrelOffset = player.GetEquipedWeaponInfo()->muzzleFlashOffset;
		glm::mat4 gunTransform = glm::translate(glm::mat4(1.0f), player.m_currentWeaponGameObject.GetPosition()) * player.m_currentWeaponGameObject.GetRotationMatrix();

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
		glBindTexture(GL_TEXTURE_2D, muzzleFlashTexture->m_id);

		if (player.m_muzzleFlashTimer > 0) {
			g_renderData.muzzleFlashMesh.RenderTexture(g_shaders.muzzleFlashShader);
			player.m_muzzleFlashTimer--;
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
		glBindTexture(GL_TEXTURE_2D, sansFontTexture->m_id);

		g_renderData.textMesh.RenderText("FPS: " + std::to_string(Window::GetFPSCount()), fpsTextX, fpsTextY, debugFontSize, glm::vec3(1.0f, 1.0f, 1.0f), g_shaders.uiShader);

		std::string playerPosText = "Player Position: (" +
			std::to_string(CameraManager::GetActiveCamera()->cameraPos.x) + ", " +
			std::to_string(CameraManager::GetActiveCamera()->cameraPos.y) + ", " +
			std::to_string(CameraManager::GetActiveCamera()->cameraPos.z) + ")";

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sansFontTexture->m_id);

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
		glBindTexture(GL_TEXTURE_2D, crosshairTexture->m_id);
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
			cubeMap.Cleanup();
		};

		g_renderFrameBuffers.postProcessingFrameBuffer.Cleanup();
		g_renderFrameBuffers.refractionFrameBuffer.Cleanup();
		g_renderFrameBuffers.mssaFrameBuffer.Cleanup();
	}
}