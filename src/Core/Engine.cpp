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


	}
}