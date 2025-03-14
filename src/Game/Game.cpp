#include "Game.h"

namespace Game {
	GameState g_gameState = {};

	void Init() {
		g_gameState = GameState::PLAYING;
		WeaponManager::Init();
	}

	void Update(double deltaTime) {
		// Weapon Animations
		Animator* glockAnimator = AssetManager::GetAnimatorByName("GlockAnimator");

		Animation* glockReloadAnimation = AssetManager::GetAnimationByName("Glock_ReloadEmpty");
		Animation* glockIdleAnimation = AssetManager::GetAnimationByName("Glock_Idle");
		Animation* glockWalkAnimation = AssetManager::GetAnimationByName("Glock_Walk");
		Animation* glockFire0Animation = AssetManager::GetAnimationByName("Glock_Fire0");
		Animation* glockDrawAnimation = AssetManager::GetAnimationByName("Glock_Draw");

		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");

		Animation* p90ReloadAnimation = AssetManager::GetAnimationByName("P90_Reload");
		Animation* p90IdleAnimation = AssetManager::GetAnimationByName("P90_Idle");
		Animation* p90WalkAnimation = AssetManager::GetAnimationByName("P90_Walk");
		Animation* p90Fire0Animation = AssetManager::GetAnimationByName("P90_Fire0");
		Animation* p90DrawAnimation = AssetManager::GetAnimationByName("P90_Draw");

		WeaponInfo* equipedWeapon = g_players[0].GetEquipedWeaponInfo();

		static std::string previousWeapon = equipedWeapon->name;
		static float drawAnimationFinishTime = 0.0f;
		static bool isDrawing = false;

		//if (equipedWeapon->name != previousWeapon) {
		//	if (equipedWeapon->name == "Glock") {
		//		glockAnimator->PlayAnimation(glockDrawAnimation);
		//	}
		//	else if (equipedWeapon->name == "P90") {
		//		p90Animator->PlayAnimation(p90DrawAnimation);
		//	}
		//	previousWeapon = equipedWeapon->name; 
		//	drawAnimationFinishTime = 0.0f;
		//	isDrawing = true;

		//	/*glockAnimator->Reset();
  //         p90Animator->Reset();*/
		//}

		g_players[0].ReloadWeapon();

	
		if (!equipedWeapon) {
			std::cout << "ERROR: No equipped weapon!" << std::endl;
		}

		if (equipedWeapon->name == "Glock") {
			/*if (Keyboard::KeyJustPressed(GLFW_KEY_R)) {
				glockAnimator->PlayAnimation(glockReloadAnimation);
			}*/

			if (Keyboard::KeyJustPressed(GLFW_KEY_F)) {
				glockAnimator->PlayAnimation(glockDrawAnimation);
				drawAnimationFinishTime = 0.0f;
				isDrawing = true;
			}

			if (g_players[0].IsMoving() && glockAnimator->GetCurrentAnimation() == glockIdleAnimation) {
				glockAnimator->PlayAnimation(glockWalkAnimation);
			}

			if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT) && glockAnimator->GetCurrentAnimation() != glockReloadAnimation) {
				glockAnimator->PlayAnimation(glockFire0Animation);

				// Get player position and forward direction
				glm::vec3 playerPos = g_players[0].getPosition();
				physx::PxVec3 physxPlayerPos = physx::PxVec3(playerPos.x, playerPos.y, playerPos.z);

				glm::vec3 cameraFront = g_players[0].camera.cameraFront; // Assuming player has a camera
				physx::PxVec3 forwardDirection = physx::PxVec3(cameraFront.x, cameraFront.y, cameraFront.z);

				// Fire the bullet
				Physics::FireBullet(physxPlayerPos + forwardDirection * 2.0f, forwardDirection, 50.0f, 50.0f);
			}

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;

				
				if (drawAnimationFinishTime >= 0.65f) {
					glockAnimator->PlayAnimation(glockIdleAnimation);
					isDrawing = false;  
				}
			}


			if (!isDrawing && glockAnimator->IsAnimationFinished() && glockAnimator->GetCurrentAnimation() != glockIdleAnimation) {
				glockAnimator->PlayAnimation(glockIdleAnimation);
			}

			glockAnimator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "P90") {
			if (Keyboard::KeyJustPressed(GLFW_KEY_R)) {
				p90Animator->PlayAnimation(p90ReloadAnimation);
			}

			if (g_players[0].IsMoving() && p90Animator->GetCurrentAnimation() == p90IdleAnimation) {
				p90Animator->PlayAnimation(p90WalkAnimation);
			}


			if (Mouse::button(GLFW_MOUSE_BUTTON_LEFT) && p90Animator->GetCurrentAnimation() != p90ReloadAnimation && p90Animator->GetCurrentAnimation() != p90Fire0Animation) {
				p90Animator->PlayAnimation(p90Fire0Animation, 2.0f);
			}

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;

			
				if (drawAnimationFinishTime >= 0.85f) {
					p90Animator->PlayAnimation(p90IdleAnimation);
					isDrawing = false;  
				}
			}


			if (!isDrawing && p90Animator->IsAnimationFinished() && p90Animator->GetCurrentAnimation() != p90IdleAnimation) {
				p90Animator->PlayAnimation(p90IdleAnimation);
			}

			p90Animator->UpdateAnimation(deltaTime);
		}

		if (equipedWeapon->name == "Glock" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
			g_players[0].EquipWeapon("P90");
			p90Animator->PlayAnimation(p90DrawAnimation);
			glockAnimator->Reset();
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}
		else if (equipedWeapon->name == "P90" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
			g_players[0].EquipWeapon("Glock");
			glockAnimator->PlayAnimation(glockDrawAnimation);
			p90Animator->Reset();
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}

		// Weapons Position
		UpdateWeaponPositionByName(equipedWeapon->name);
		
	}

	void CreatePlayers() {
		Player player(glm::vec3(0.0f, 1.8f, 0.0f), 2.3f, 75.0f);
		player.EquipWeapon("Glock");

		g_players.push_back(player);
	}

	Player& GetPLayerByIndex(int index) {
		if (index < 0 || index >= g_players.size()) {
			throw std::out_of_range("ERROR::GetPlayerByIndex::Index out of range!");
		}
		return g_players[index];
	}

	void UpdateWeaponPositionByName(std::string name) {
		Model* weaponModel = AssetManager::GetModelByName(name);

		glm::vec3 gunPosition = g_players[0].getPosition() +
			(g_players[0].camera.cameraFront * 0.7f) +   // Offset forward
			(g_players[0].camera.cameraUp * -3.85f);    // Offset downward

		// Calculate gun rotation to align with the camera
		glm::mat4 gunRotation = glm::mat4(1.0f);

		// Rotate around camera right (tilt up/down)
		float theta = acos(glm::dot(g_players[0].camera.worldUp, g_players[0].camera.cameraFront) /
			(glm::length(g_players[0].camera.worldUp) * glm::length(g_players[0].camera.cameraFront)));
		gunRotation = glm::rotate(gunRotation, glm::half_pi<float>() - theta, g_players[0].camera.cameraRight);

		// Rotate around camera up (align gun to camera facing direction in horizontal plane)
		glm::vec2 front2D = glm::vec2(g_players[0].camera.cameraFront.x, g_players[0].camera.cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), glm::normalize(front2D)));
		gunRotation = glm::rotate(gunRotation, g_players[0].camera.cameraFront.z < 0 ? theta : -theta, g_players[0].camera.worldUp);

		// Apply local rotation adjustment to align gun's forward direction
		glm::mat4 localRotationFix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Adjust for left-facing gun
		localRotationFix = glm::rotate(localRotationFix, glm::radians(5.0f), glm::vec3(1.9f, 0.0f, 0.0f)); // Slight upward pitch adjustment
		if (name == "P90") {
			localRotationFix = glm::rotate(localRotationFix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		gunRotation = gunRotation * localRotationFix;

		// Apply transformations to the gun
		weaponModel->setPosition(gunPosition);
		weaponModel->setRotation(gunRotation);
	}

	GameState GetGameState() {
		return g_gameState;
	}

	void SetGameState(GameState newState) {
		g_gameState = newState;
	}
}