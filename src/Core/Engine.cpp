#include "Engine.h"

namespace Engine {
	void Run() {
		Window::Init();
		Physics::Init();
		AudioManager::Init();
		UIManager::Init();

		LoadResources();

		Game::Init();

		Player& player = Game::GetPLayerByIndex(0);

		Window::ShowWindow();

		Scene::LoadSceneFromFile("resources/levels/poolrooms.json");
		Scene::CreateHardcodedObjects();

		CameraManager::AddCamera(&player.m_camera);

		OpenGLRenderer::Init();
		Editor::Init();
		EditorPanel::Init();

		CameraManager::AddCamera(&Editor::GetCamera());

		while (!Window::WindowShouldClose()) {
			Window::UpdateDeltaTime();
			Window::UpdateFPSCount();

			Physics::BeginFrame();

			if (Keyboard::KeyJustPressed(GLFW_KEY_F1)) {
				if (Game::GetGameState() == Game::GameState::EDITOR) {
					Game::SetGameState(Game::GameState::PLAYING);
				}
				else {
					Game::SetGameState(Game::GameState::EDITOR);
				}
			}

			if (Game::GetGameState() == Game::GameState::PLAYING) {
			   CameraManager::SetActiveCamera(0);
			   Game::Update(Window::GetDeltaTime());

			   glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				CameraManager::SetActiveCamera(1);
				Editor::Update();
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			Window::PrepareFrame();
			AudioManager::Update();
			EditorPanel::Update();
			CameraManager::Update();
			OpenGLRenderer::Render();
			UIManager::Update();

			Mouse::ClearDeltas();
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
		AudioManager::LoadAudio("slosh1.wav");
		AudioManager::LoadAudio("slosh2.wav");
		AudioManager::LoadAudio("slosh3.wav");
		AudioManager::LoadAudio("slosh4.wav");
		AudioManager::LoadAudio("BulletCasingBounce.wav");
		AudioManager::LoadAudio("Flesh_Bloody_Break.wav");
		AudioManager::LoadAudio("Blood_Splash.wav");

		// fonts
		Text2D::LoadFont("resources/fonts/sans.fnt");
		AssetManager::LoadTexture("resources/fonts", "sans.png", aiTextureType_DIFFUSE);

		// exr
		AssetManager::LoadEXRTexture("resources/exr", "blood_pos4.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_pos6.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_pos7.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_pos9.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_norm4.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_norm6.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_norm7.exr");
		AssetManager::LoadEXRTexture("resources/exr", "blood_norm9.exr");

		// textures
		AssetManager::LoadAllTexturesAsync();

		// models
		auto start = std::chrono::high_resolution_clock::now();
		AssetManager::LoadSkinnedModel("P90", "resources/models/P90.fbx");
		AssetManager::LoadSkinnedModel("Glock", "resources/models/Glock.fbx");
		AssetManager::LoadSkinnedModel("AKS74U", "resources/models/AKS74U_Simple.fbx");
		AssetManager::LoadSkinnedModel("Katana", "resources/models/Katana.fbx");
		AssetManager::LoadSkinnedModel("Mannequin", "resources/models/Mannequin.fbx");
		AssetManager::LoadSkinnedModel("Barrel", "resources/models/Barrel.obj");
		AssetManager::LoadSkinnedModel("Hanging_Light", "resources/models/Hanging_Light.obj");
		AssetManager::LoadSkinnedModel("Ceiling_Light", "resources/models/CeilingLamp.obj");
		AssetManager::LoadSkinnedModel("Bullet_Case_9mm", "resources/models/Bullet_Case_9mm.obj");
		AssetManager::LoadSkinnedModel("Bullet_Case_Rifle", "resources/models/Bullet_Case_Rifle.obj");
		AssetManager::LoadSkinnedModel("blood_mesh4", "resources/models/blood_mesh4.obj");
		AssetManager::LoadSkinnedModel("blood_mesh6", "resources/models/blood_mesh6.obj");
		AssetManager::LoadSkinnedModel("blood_mesh7", "resources/models/blood_mesh7.obj");
		AssetManager::LoadSkinnedModel("blood_mesh9", "resources/models/blood_mesh9.obj");
		AssetManager::LoadSkinnedModel("AKS74U_Pickup", "resources/models/AKS74U_Pickup.obj");

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = end - start;

		std::cout << "Loaded models in: " << duration.count() << " seconds\n";

		AssetManager::LoadModel("Cube", ModelType::CUBE);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE);
		AssetManager::LoadModel("Plane", ModelType::PLANE);
		AssetManager::LoadModel("WaterPlane", ModelType::PLANE);

		// animations
		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");
		Model* aks74uModel = AssetManager::GetModelByName("AKS74U");
		Model* katanaModel = AssetManager::GetModelByName("Katana");
		Model* mannequinModel = AssetManager::GetModelByName("Mannequin");

		AssetManager::LoadAnimation("AKS74U_Idle", "resources/animations/AKS74U_Idle.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_Reload", "resources/animations/AKS74U_Reload.fbx", aks74uModel);
		AssetManager::LoadAnimation("AKS74U_ReloadEmpty", "resources/animations/AKS74U_ReloadEmpty.fbx", aks74uModel);
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
		AssetManager::LoadAnimation("Glock_Spawn", "resources/animations/Glock_Spawn.fbx", glockModel);

		AssetManager::LoadAnimation("Knife_Idle", "resources/animations/Knife_Idle.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing0", "resources/animations/Knife_Swing0.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing1", "resources/animations/Knife_Swing1.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Swing2", "resources/animations/Knife_Swing2.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Draw", "resources/animations/Knife_Draw.fbx", katanaModel);
		AssetManager::LoadAnimation("Knife_Walk", "resources/animations/Knife_Walk.fbx", katanaModel);

		AssetManager::LoadAnimation("P90_Idle", "resources/animations/P90_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Reload", "resources/animations/P90_Reload.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ReloadEmpty", "resources/animations/P90_ReloadEmpty.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Walk", "resources/animations/P90_Walk.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Fire0", "resources/animations/P90_Fire0.fbx", p90Model);
		AssetManager::LoadAnimation("P90_Draw", "resources/animations/P90_Draw.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_In", "resources/animations/P90_ADS_In.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Out", "resources/animations/P90_ADS_Out.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Idle", "resources/animations/P90_ADS_Idle.fbx", p90Model);
		AssetManager::LoadAnimation("P90_ADS_Fire", "resources/animations/P90_ADS_Fire0.fbx", p90Model);
	
		AssetManager::LoadAnimation("Mannequin_Run", "resources/models/Mannequin.fbx", mannequinModel);
		AssetManager::LoadAnimation("Mannequin_Death", "resources/animations/Mannequin_Death.fbx", mannequinModel);

		AssetManager::LoadAnimator("GlockAnimator", AssetManager::GetAnimationByName("Glock_Idle"));
		AssetManager::LoadAnimator("P90Animator", AssetManager::GetAnimationByName("P90_Idle"));
		AssetManager::LoadAnimator("AKS74UAnimator", AssetManager::GetAnimationByName("AKS74U_Idle"));
		AssetManager::LoadAnimator("KatanaAnimator", AssetManager::GetAnimationByName("Knife_Idle"));
		AssetManager::LoadAnimator("MannequinAnimator", AssetManager::GetAnimationByName("Mannequin_Run"));

		AssetManager::BuildMaterials();
	}
}