#include "Engine.h"

namespace Engine {

	void Run() {
		Window::Init();
		Physics::Init();
		AudioManager::Init();
		Game::Init();
		Game::CreatePlayers();

		Player& player = Game::GetPLayerByIndex(0);

		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 10.0f, 1.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);

		LoadResources();

		OpenGLRenderer::Init();
		Editor::Init();
		EditorPanel::Init();

		CameraManager::AddCamera(&player.m_camera);
		CameraManager::AddCamera(&Editor::GetCamera());

		while (!Window::WindowShouldClose()) {
			Window::UpdateDeltaTime();
			Window::UpdateFPSCount();

			if (Keyboard::KeyJustPressed(GLFW_KEY_F1)) {
				if (Game::GetGameState() == Game::GameState::EDITOR) {
					Game::SetGameState(Game::GameState::PLAYING);
				}
				else {
					Game::SetGameState(Game::GameState::EDITOR);
				}
			}

			Window::ProcessInput(Window::GetDeltaTime());
			Physics::Simulate(Window::GetDeltaTime());

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			CameraManager::UpdateViewMatrix();
			CameraManager::UpdateProjectionMatrix();

			if (Game::GetGameState() == Game::GameState::PLAYING) {
				CameraManager::SetActiveCamera(0);
				player.Update(Window::GetDeltaTime());
				Game::Update(Window::GetDeltaTime());

				Scene::GetGameObjectByName("Cube0")->SetPosition(cubeTransformData.position);
				Scene::GetGameObjectByName("Cube0")->SetRotationMatrix(rotationMatrix);

				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				CameraManager::SetActiveCamera(1);
				Editor::UpdateCamera();
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			Window::PrepareFrame();
			AudioManager::Update();
			EditorPanel::Update();
			OpenGLRenderer::RenderFrame();
			EditorPanel::Render();
			Window::ProcessEvents();
		}

		EditorPanel::Cleanup();
		CameraManager::Clear();
		AssetManager::CleanupModels();
		AudioManager::Cleanup();
		OpenGLRenderer::Cleanup();
		Physics::CleanupPhysX();
		Window::ShutDown();
	}

	void LoadResources() {
		// audio
		AudioManager::LoadAudio("Glock_Fire0.wav");
		AudioManager::LoadAudio("Glock_Fire1.wav");
		AudioManager::LoadAudio("Glock_Fire2.wav");
		AudioManager::LoadAudio("Glock_Fire3.wav");
		AudioManager::LoadAudio("Glock_Reload.wav");
		AudioManager::LoadAudio("Glock_ReloadEmpty.wav");
		AudioManager::LoadAudio("P90_Fire0.wav");
		AudioManager::LoadAudio("P90_Fire1.wav");
		AudioManager::LoadAudio("P90_Fire2.wav");
		AudioManager::LoadAudio("P90_Fire3.wav");
		AudioManager::LoadAudio("P90_Reload.wav");
		AudioManager::LoadAudio("P90_ReloadEmpty.wav");
		AudioManager::LoadAudio("AKS74U_Fire0.wav");
		AudioManager::LoadAudio("AKS74U_Fire1.wav");
		AudioManager::LoadAudio("AKS74U_Fire2.wav");
		AudioManager::LoadAudio("AKS74U_Fire3.wav");
		AudioManager::LoadAudio("AKS74U_Reload.wav");
		AudioManager::LoadAudio("AKS74U_ReloadEmpty.wav");
		AudioManager::LoadAudio("NextWeapon.wav");
		AudioManager::LoadAudio("Katana_Hit1.wav");
		AudioManager::LoadAudio("Katana_Draw.wav");

		// fonts
		Text2D::LoadFont("resources/fonts/sans.fnt");
		AssetManager::LoadTexture("sans.png", aiTextureType_DIFFUSE);

		// textures
		AssetManager::LoadAllTexturesAsync();

		// models
		ModelCreateInfo glockCreateInfo{};
		ModelCreateInfo p90CreateInfo{};
		ModelCreateInfo aks74uCreateInfo{};
		ModelCreateInfo katanaCreateInfo{};
		ModelCreateInfo cubeCreateInfo{
			"PoolTile_ALB.png",
			"PoolTile_NRM.png",
			"PoolTile_RMA.png"
		};
		ModelCreateInfo lampCreateInfo{};
		ModelCreateInfo planeCreateInfo{
			"PoolTile_ALB.png",
			"PoolTile_NRM.png",
			"PoolTile_RMA.png"
		};
		ModelCreateInfo waterPlaneCreateInfo{
			"WaterDUDV.png",
			"WaterNormal.png",
		};

		AssetManager::LoadAssimpModel("P90", "resources/models/P90T.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);
		AssetManager::LoadAssimpModel("AKS74U", "resources/models/AKS74UBfbx.fbx", aks74uCreateInfo);
		AssetManager::LoadAssimpModel("Katana", "resources/models/untitled.fbx", katanaCreateInfo);
		AssetManager::LoadAssimpModel("DEAGLE", "resources/models/Deagle_Animation.fbx", aks74uCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);
		AssetManager::LoadModel("WaterPlane", ModelType::PLANE, waterPlaneCreateInfo);

		// animations
		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");
		Model* aks74uModel = AssetManager::GetModelByName("AKS74U");
		Model* katanaModel = AssetManager::GetModelByName("Katana");
		Model* dEagleModel = AssetManager::GetModelByName("DEAGLE");

		auto start = std::chrono::high_resolution_clock::now();

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

		AssetManager::LoadAnimation("Knife_Idle", "resources/animations/Knife_Idle.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing0", "resources/animations/Knife_Swing0.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing1", "resources/animations/Knife_Swing1.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing2", "resources/animations/Knife_Swing2.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Draw", "resources/animations/Knife_Draw.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Walk", "resources/animations/Knife_Walk.fbx", katanaModel);

		AssetManager::LoadAnimation("DEAGLE_Walk", "resources/models/Deagle_Animation.fbx", dEagleModel);

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
		AssetManager::LoadAnimator("DEAGLEAnimator", AssetManager::GetAnimationByName("DEAGLE_Walk"));
		AssetManager::LoadAnimator("KatanaAnimator", AssetManager::GetAnimationByName("Knife_Idle"));

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;

		std::cout << "Loaded animations in: " << duration.count() << " seconds\n";
	}
}