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
		Animation* glockADSInAnimation = AssetManager::GetAnimationByName("Glock_ADS_In");
		Animation* glockADSOutAnimation = AssetManager::GetAnimationByName("Glock_ADS_Out");
		Animation* glockADSIdleAnimation = AssetManager::GetAnimationByName("Glock_ADS_Idle");
		Animation* glockADSFire1Animation = AssetManager::GetAnimationByName("Glock_ADS_Fire1");

		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");
		

		Animation* p90ReloadAnimation = AssetManager::GetAnimationByName("P90_Reload");
		Animation* p90IdleAnimation = AssetManager::GetAnimationByName("P90_Idle");
		Animation* p90WalkAnimation = AssetManager::GetAnimationByName("P90_Walk");
		Animation* p90Fire0Animation = AssetManager::GetAnimationByName("P90_Fire0");
		Animation* p90DrawAnimation = AssetManager::GetAnimationByName("P90_Draw");

		Animator* aks74uAnimator = AssetManager::GetAnimatorByName("AKS74UAnimator");

		Animation* aks74uIdleAnimation = AssetManager::GetAnimationByName("AKS74U_Idle");
		Animation* aks74uDrawAnimation = AssetManager::GetAnimationByName("AKS74U_Draw");
		Animation* aks74uWalkAnimation = AssetManager::GetAnimationByName("AKS74U_Walk");

		WeaponInfo* equipedWeapon = g_players[0].GetEquipedWeaponInfo();

		static std::string previousWeapon = equipedWeapon->name;
		static float drawAnimationFinishTime = 0.0f;
		static float ADSInAnimationFinishTime = 0.0f;
		static float ADSOutAnimationFinishTime = 0.0f;

		static bool isDrawing = false;
		static bool isInADS = false;

		g_players[0].ReloadWeapon();
		g_players[0].FireWeapon();
	
		if (!equipedWeapon) {
			std::cout << "ERROR: No equipped weapon!" << std::endl;
		}

		if (equipedWeapon->name == "Glock") {
			if (Keyboard::KeyJustPressed(GLFW_KEY_F)) {
				glockAnimator->PlayAnimation(glockDrawAnimation);
				drawAnimationFinishTime = 0.0f;
				isDrawing = true;
			}

			if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_RIGHT)) {
				glockAnimator->PlayAnimation(glockADSInAnimation);
				ADSInAnimationFinishTime = 0.0f;
				isInADS = true;
			}


			if (g_players[0].IsMoving() && glockAnimator->GetCurrentAnimation() == glockIdleAnimation) {
				glockAnimator->PlayAnimation(glockWalkAnimation);
			}

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;

				
				if (drawAnimationFinishTime >= 0.65f) {
					glockAnimator->PlayAnimation(glockIdleAnimation);
					isDrawing = false;  
				}
			}

			if (isInADS && Mouse::button(GLFW_MOUSE_BUTTON_RIGHT) && glockAnimator->GetCurrentAnimation() != glockADSFire1Animation) {
				ADSInAnimationFinishTime += deltaTime;
				if (ADSInAnimationFinishTime >= 0.45f) {
					glockAnimator->PlayAnimation(glockADSIdleAnimation);
				}
			}

			if (isInADS && Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT)) {
				std::cout << "FUNCTION TRIGGERRRED!!!" << std::endl;
				glockAnimator->PlayAnimation(glockADSFire1Animation);
			}

			if (isInADS && Mouse::buttonWentUp(GLFW_MOUSE_BUTTON_RIGHT)) {
				glockAnimator->PlayAnimation(glockADSOutAnimation);
				ADSOutAnimationFinishTime = 0.0f;
				isInADS = false;
		
			}

			if (!isDrawing && !isInADS && glockAnimator->GetCurrentAnimation() == glockADSOutAnimation) {
				ADSOutAnimationFinishTime += deltaTime;
				if (ADSOutAnimationFinishTime >= 0.202) {
					glockAnimator->PlayAnimation(glockIdleAnimation);
				}
			} else if (!isDrawing && glockAnimator->IsAnimationFinished() && glockAnimator->GetCurrentAnimation() != glockIdleAnimation) {
				glockAnimator->PlayAnimation(glockIdleAnimation);
			}

			glockAnimator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "AKS74U") {
			if (g_players[0].IsMoving() && aks74uAnimator->GetCurrentAnimation() == aks74uIdleAnimation) {
				aks74uAnimator->PlayAnimation(aks74uWalkAnimation, 0.5f);
			}

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;


				if (drawAnimationFinishTime >= 0.65f) {
					aks74uAnimator->PlayAnimation(aks74uIdleAnimation);
					isDrawing = false;
				}
			}

			if (!isDrawing && aks74uAnimator->IsAnimationFinished() && aks74uAnimator->GetCurrentAnimation() != aks74uIdleAnimation) {
				aks74uAnimator->PlayAnimation(aks74uIdleAnimation);
			}


			aks74uAnimator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "P90") {
			if (Keyboard::KeyJustPressed(GLFW_KEY_R)) {
				p90Animator->PlayAnimation(p90ReloadAnimation);
			}

			if (g_players[0].IsMoving() && p90Animator->GetCurrentAnimation() == p90IdleAnimation) {
				p90Animator->PlayAnimation(p90WalkAnimation);
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
			g_players[0].EquipWeapon("AKS74U");
			aks74uAnimator->PlayAnimation(aks74uDrawAnimation, 1.5);
		/*	glockAnimator->Reset();*/
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}
		else if (equipedWeapon->name == "AKS74U" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
			g_players[0].EquipWeapon("P90");
			p90Animator->PlayAnimation(p90DrawAnimation);
			/*p90Animator->Reset();*/
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}
		else if (equipedWeapon->name == "P90" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
			g_players[0].EquipWeapon("Glock");
			glockAnimator->PlayAnimation(glockDrawAnimation);
			/*p90Animator->Reset();*/
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}

		// Weapons Position
		if (glockAnimator->GetCurrentAnimation() == glockADSInAnimation ||
			glockAnimator->GetCurrentAnimation() == glockADSIdleAnimation ||
			glockAnimator->GetCurrentAnimation() == glockADSOutAnimation || 
			glockAnimator->GetCurrentAnimation() == glockADSFire1Animation) {
			UpdateWeaponPositionByName(equipedWeapon->name, true);
		}
		else {
			UpdateWeaponPositionByName(equipedWeapon->name);
		}
		
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

	void UpdateWeaponPositionByName(std::string name, bool flipRotation) {
		Model* weaponModel = AssetManager::GetModelByName(name);


		glm::vec3 gunPosition;

		if (name == "Glock") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].camera.cameraUp * -4.10f);    // Offset downward
		} else if (name == "AKS74U") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].camera.cameraUp * -4.2f);    // Offset downward
		}
		else {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].camera.cameraUp * -3.8f);    // Offset downward
		}

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
		if (name == "P90" || flipRotation) {
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