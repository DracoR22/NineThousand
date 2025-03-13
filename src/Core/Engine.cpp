#include "Engine.h"

namespace Engine {

	void Run() {
		Window::Init();
		Physics::InitPhysx();
		Game::Init();
		Game::CreatePlayers();

		Player& player = Game::GetPLayerByIndex(0);;

		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 10.0f, 1.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);

		OpenGLRenderer::Init();

	
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		double lastTime = glfwGetTime();
		
		int fps = 0;
		const int FPS_SMOOTHING_SAMPLES = 50; // Number of frames to average
		double fpsBuffer[FPS_SMOOTHING_SAMPLES] = { 0 };
		int fpsIndex = 0;

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

				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			Window::PrepareFrame();

			// IMGUI
			float gamma = OpenGLRenderer::GetGammaValue();
			RendererCommon::PostProcessMode postProcessMode = OpenGLRenderer::GetPostProcessMode();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Game Settings");
			ImGui::Text("FPS: %d", fps);
			ImGui::Text("Player Position: (%.2f, %.2f, %.2f)", player.getPosition().x, player.getPosition().y, player.getPosition().z);

			if (ImGui::SliderFloat("Gamma", &gamma, 0.1f, 5.0f, "%.2f")) {
				OpenGLRenderer::ChangeGammaValue(gamma);
			}

			if (ImGui::BeginCombo("Post Processing Mode", postProcessMode == RendererCommon::PostProcessMode::NONE ? "None" : "Sharpen")) {
				if (ImGui::Selectable("None", postProcessMode == RendererCommon::PostProcessMode::NONE)) {
					OpenGLRenderer::ChangePostProcessMode(RendererCommon::PostProcessMode::NONE);
				}
				if (ImGui::Selectable("Sharpen", postProcessMode == RendererCommon::PostProcessMode::SHARPEN)) {
					OpenGLRenderer::ChangePostProcessMode(RendererCommon::PostProcessMode::SHARPEN);
				}
				ImGui::EndCombo();
			}

			ImGui::End();

			OpenGLRenderer::RenderFrame();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			Window::ProcessEvents();
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		AssetManager::CleanupModels();
		OpenGLRenderer::Cleanup();

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}