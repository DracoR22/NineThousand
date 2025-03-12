#include "Engine.h"

namespace Engine {

	struct Shaders {
		Shader texturedObjectShader;
		Shader skyboxShader;
		Shader animShader;
		Shader lampShader;
		Shader shadowMapShader;
		Shader weaponShader;
		Shader frameBufferShader;
		Shader postProcessShader;
	} _shaders;

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	enum class PostProcessMode {
		NONE,
		SHARPEN
	};

	PostProcessMode g_currentMode = PostProcessMode::NONE;


	void Run() {
		Window::Init();
		Physics::InitPhysx();
		Game::Init();
		Game::CreatePlayers();

		Player& player = Game::GetPLayerByIndex(0);

		float rotationAngle = 0.0f;

		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 10.0f, 1.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);

		// load shaders
		_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
		_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
		_shaders.animShader.load("animated.vert", "animated.frag");
		_shaders.lampShader.load("lamp.vert", "lamp.frag");
		_shaders.shadowMapShader.load("shadow_map.vert", "shadow_map.frag");
		_shaders.weaponShader.load("weapon.vert", "weapon.frag");
		_shaders.frameBufferShader.load("frame_buffer.vert", "frame_buffer.frag");
		_shaders.postProcessShader.load("post_process.vert", "post_process.frag");

		// load skybox
		CubeMap cubemap;

		std::vector<std::string> faces{
			"resources/textures/SkyRight.jpg",
			"resources/textures/SkyLeft.jpg",
			"resources/textures/SkyTop.jpg",
			"resources/textures/SkyBottom.jpg",
			"resources/textures/SkyFront.jpg",
			"resources/textures/SkyBack.jpg",
		};

		cubemap.loadTextures(faces);
		cubemap.init();

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

		float quadVertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		AssetManager::LoadAssimpModel("P90", "resources/models/P90T.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);

		// Quad For FrameBuffer
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);

		std::vector<PointLight> sceneLights;

		PointLight cubeLampLight;
		cubeLampLight.position = AssetManager::GetModelByName("CubeLamp")->pos;
		cubeLampLight.constant = 1.0f;
		cubeLampLight.linear = 0.0014f;
		cubeLampLight.quadratic = 0.000007f;
		cubeLampLight.ambient = glm::vec3(0.02f);
		cubeLampLight.diffuse = glm::vec3(0.8f);
		cubeLampLight.specular = glm::vec3(1.0f);

		sceneLights.push_back(cubeLampLight);

		// Create frame buffers
		FrameBuffer frameBuffer;
		frameBuffer.Create(Window::currentWidth, Window::currentHeight);
		frameBuffer.CreateAttachment();

		// Load shadow map
		ShadowMap shadowMap;
		shadowMap.Init();

		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");

		// load animations
		AssetManager::LoadAnimation("Glock_Idle", "resources/animations/Glock_Idle.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Reload", "resources/animations/Glock_Reload.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Walk", "resources/animations/Glock_Walk.fbx", glockModel);
		AssetManager::LoadAnimation("Glock_Fire0", "resources/animations/Glock_Fire0.fbx", glockModel);

		AssetManager::LoadAnimation("P90_Idle", "resources/animations/P90_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Reload", "resources/animations/P90_Reload.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Walk", "resources/animations/P90_Walk.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Fire0", "resources/animations/P90_Fire0.fbx", p90Model);

		AssetManager::LoadAnimator("GlockAnimator", AssetManager::GetAnimationByName("Glock_Idle"));
		Animator* glockAnimator = AssetManager::GetAnimatorByName("GlockAnimator");

		AssetManager::LoadAnimator("P90Animator", AssetManager::GetAnimationByName("P90_Idle"));
		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		double lastTime = glfwGetTime();
		
		int fps = 0;
		const int FPS_SMOOTHING_SAMPLES = 50; // Number of frames to average
		double fpsBuffer[FPS_SMOOTHING_SAMPLES] = { 0 };
		int fpsIndex = 0;
		float gamma = 2.2f;

		// IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);          // Background
		colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);            // Title Bar
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);      // Active Title
		colors[ImGuiCol_Button] = ImVec4(0.0f, 0.5f, 0.8f, 1.0f);             // Buttons
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.6f, 0.9f, 1.0f);      // Button Hover
		colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.7f, 1.0f);       // Button Click
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);         // Input Box Background
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);     // Input Hover
		colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);             // Panel Borders

		ImGui_ImplGlfw_InitForOpenGL(Window::window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		while (!Window::WindowShouldClose()) {
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			/*deltaTime = glm::min(deltaTime, 0.05f);*/

			if (deltaTime > 0.0) {
				double currentFPS = 1.0 / deltaTime;

				// Store FPS in the buffer
				fpsBuffer[fpsIndex] = currentFPS;
				fpsIndex = (fpsIndex + 1) % FPS_SMOOTHING_SAMPLES;

				// Compute the moving average FPS
				double fpsSum = 0.0;
				for (int i = 0; i < FPS_SMOOTHING_SAMPLES; i++) {
					fpsSum += fpsBuffer[i];
				}
				fps = fpsSum / FPS_SMOOTHING_SAMPLES;
			}

			if (Keyboard::KeyJustPressed(GLFW_KEY_F1)) {
				if (Game::GetGameState() == Game::GameState::EDITOR) {
					Game::SetGameState(Game::GameState::PLAYING);
				}
				else {
					Game::SetGameState(Game::GameState::EDITOR);
				}
			}

			Window::ProcessInput(deltaTime);
			Physics::Simulate(deltaTime);

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			if (Game::GetGameState() == Game::GameState::PLAYING) {
				player.processInput(deltaTime);
				Game::Update(deltaTime);

			    AssetManager::GetModelByName("Cube")->setPosition(cubeTransformData.position);
			    AssetManager::GetModelByName("Cube")->setRotation(rotationMatrix);
			}

			if (Game::GetGameState() == Game::GameState::PLAYING) {
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			if (Keyboard::KeyJustPressed(GLFW_KEY_2)) {
				_shaders.texturedObjectShader.load("textured_obj.vert", "textured_obj.frag");
				_shaders.skyboxShader.load("skybox.vert", "skybox.frag");
				_shaders.animShader.load("animated.vert", "animated.frag");
				_shaders.lampShader.load("lamp.vert", "lamp.frag");
				_shaders.weaponShader.load("weapon.vert", "weapon.frag");
				_shaders.frameBufferShader.load("frame_buffer.vert", "frame_buffer.frag");

			}

			Window::PrepareFrame();

			// IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Game Settings");
			ImGui::Text("FPS: %d", fps);
			ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", player.getPosition().x, player.getPosition().y, player.getPosition().z);
			ImGui::SliderFloat("Gamma", &gamma, 0.1f, 5.0f, "%.2f");

			if (ImGui::BeginCombo("Post Processing Mode", g_currentMode == PostProcessMode::NONE ? "None" : "Sharpen")) {
				if (ImGui::Selectable("None", g_currentMode == PostProcessMode::NONE)) {
					g_currentMode = PostProcessMode::NONE;
				}
				if (ImGui::Selectable("Sharpen", g_currentMode == PostProcessMode::SHARPEN)) {
					g_currentMode = PostProcessMode::SHARPEN;
				}
				ImGui::EndCombo();
			}

			ImGui::End();
			
			/*p90Animator.UpdateAnimation(deltaTime);*/

			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = player.camera.getViewMatrix();
			projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);

			// ------ SHADOW PASS (Render to Depth Map) ------
			glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
			glm::mat4 lightView = glm::lookAt(sceneLights[0].position, AssetManager::GetModelByName("Cube")->pos, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProjection = orthogonalProjection * lightView;

			shadowMap.Clear();

			glCullFace(GL_FRONT);

			_shaders.shadowMapShader.activate();
			_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);
			AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

			glCullFace(GL_BACK);

			shadowMap.Unbind();
			glViewport(0, 0, Window::currentWidth, Window::currentHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// ------ BIND FRAME BUFFERS -------
			if (frameBuffer.GetWidth() != Window::currentWidth || frameBuffer.GetHeight() != Window::currentHeight) {
				frameBuffer.Resize(Window::currentWidth, Window::currentHeight);
			}

			frameBuffer.Bind();
			glViewport(0, 0, Window::currentWidth, Window::currentHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// ------ RENDER PASS ------
			_shaders.weaponShader.activate();
			_shaders.weaponShader.setMat4("view", view);
			_shaders.weaponShader.setMat4("projection", projection);
			_shaders.weaponShader.setMat4("lightProjection", lightProjection);
			_shaders.weaponShader.setInt("noPointLights", sceneLights.size());
			_shaders.weaponShader.setFloat("gamma", gamma);
			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

				_shaders.weaponShader.setVec3(lightUniform + ".position", sceneLights[i].position);
				_shaders.weaponShader.setFloat(lightUniform + ".constant", sceneLights[i].constant);
				_shaders.weaponShader.setFloat(lightUniform + ".linear", sceneLights[i].linear);
				_shaders.weaponShader.setFloat(lightUniform + ".quadratic", sceneLights[i].quadratic);

				_shaders.weaponShader.setVec3(lightUniform + ".ambient", sceneLights[i].ambient);
				_shaders.weaponShader.setVec3(lightUniform + ".diffuse", sceneLights[i].diffuse);
				_shaders.weaponShader.setVec3(lightUniform + ".specular", sceneLights[i].specular);
			}
			auto transforms = glockAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.weaponShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("Glock", _shaders.weaponShader);

			_shaders.animShader.activate();
			_shaders.animShader.setMat4("view", view);
			_shaders.animShader.setMat4("projection", projection);
			_shaders.animShader.setFloat("gamma", gamma);
			auto p90Transforms = p90Animator->GetFinalBoneMatrices();
			for (int i = 0; i < p90Transforms.size(); ++i)
				_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", p90Transforms[i]);
			AssetManager::DrawModel("P90", _shaders.animShader);

			_shaders.texturedObjectShader.activate();
			_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
			_shaders.texturedObjectShader.setMat4("view", view);
			_shaders.texturedObjectShader.setMat4("projection", projection);
			_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
			_shaders.texturedObjectShader.setInt("noPointLights", sceneLights.size());
			_shaders.texturedObjectShader.setInt("shadowMap", 3);
			_shaders.texturedObjectShader.setFloat("gamma", gamma);
			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

				_shaders.texturedObjectShader.setVec3(lightUniform + ".position", sceneLights[i].position);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", sceneLights[i].constant);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", sceneLights[i].linear);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", sceneLights[i].quadratic);

				_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", sceneLights[i].ambient);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", sceneLights[i].diffuse);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", sceneLights[i].specular);
			}

			/*Scene::GetPrimitiveModelByName("Plane")->draw(_shaders.texturedObjectShader);*/
			AssetManager::DrawModel("Plane", _shaders.texturedObjectShader);

			_shaders.texturedObjectShader.activate();
			_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
			_shaders.texturedObjectShader.setMat4("view", view);
			_shaders.texturedObjectShader.setMat4("projection", projection);
			_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
			_shaders.texturedObjectShader.setInt("noPointLights", sceneLights.size());
			_shaders.texturedObjectShader.setInt("shadowMap", 3);
			_shaders.texturedObjectShader.setFloat("gamma", gamma);
			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

				_shaders.texturedObjectShader.setVec3(lightUniform + ".position", sceneLights[i].position);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", sceneLights[i].constant);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", sceneLights[i].linear);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", sceneLights[i].quadratic);

				_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", sceneLights[i].ambient);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", sceneLights[i].diffuse);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", sceneLights[i].specular);
			}
			/*Scene::GetPrimitiveModelByName("Cube")->draw(_shaders.texturedObjectShader);*/
			AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

			_shaders.lampShader.activate();
			_shaders.lampShader.set3Float("viewPos", player.getPosition());
			_shaders.lampShader.setMat4("view", view);
			_shaders.lampShader.setMat4("projection", projection);
			_shaders.lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			/*Scene::GetPrimitiveModelByName("CubeLamp")->draw(_shaders.lampShader);*/
			AssetManager::DrawModel("CubeLamp", _shaders.lampShader);

			cubemap.render(_shaders.skyboxShader, player.camera.getViewMatrix(), projection);


			// ------ FRAME BUFFER PASS -----------

			/*glActiveTexture(GL_TEXTURE0);*/
			frameBuffer.Unbind();
			
			switch (g_currentMode)
			{
			case Engine::PostProcessMode::NONE:
				_shaders.postProcessShader.activate();
				_shaders.postProcessShader.setInt("screenTexture", 0);
				break;
			case Engine::PostProcessMode::SHARPEN:
				_shaders.frameBufferShader.activate();
				_shaders.frameBufferShader.setInt("screenTexture", 0);
				break;
			default:
				_shaders.postProcessShader.activate();
				_shaders.postProcessShader.setInt("screenTexture", 0);
				break;
			}

			glBindVertexArray(quadVAO);
			glDisable(GL_DEPTH_TEST);

			glActiveTexture(GL_TEXTURE0);
			frameBuffer.BindTexture();

			glDrawArrays(GL_TRIANGLES, 0, 6);


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			Window::ProcessEvents();
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		shadowMap.Cleanup();
		AssetManager::CleanupModels();
		cubemap.cleanup();
		frameBuffer.Cleanup();
		/*Scene::GetPrimitiveModelByName("Plane")->cleanup();*/

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}