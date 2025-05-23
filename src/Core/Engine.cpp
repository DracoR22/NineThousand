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

		Text2D::LoadFont("resources/fonts/sans.fnt");
		AssetManager::LoadTexture("sans.png", aiTextureType_DIFFUSE);
		AssetManager::LoadAllTexturesAsync();

		OpenGLRenderer::Init();
		EditorUI::Init();

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
				player.processInput(Window::GetDeltaTime());
				Game::Update(Window::GetDeltaTime());

			    AssetManager::GetModelByName("Cube")->setPosition(cubeTransformData.position);
			    AssetManager::GetModelByName("Cube")->setRotation(rotationMatrix);


				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(Window::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			Window::PrepareFrame();
			AudioManager::Update();
			EditorUI::Update();
			OpenGLRenderer::RenderFrame();
			EditorUI::Render();
			Window::ProcessEvents();
		}

		EditorUI::Cleanup();
		AssetManager::CleanupModels();
		AudioManager::Cleanup();
		OpenGLRenderer::Cleanup();
		Physics::CleanupPhysX();
		Window::ShutDown();
	}
}