#include "Game.h"

namespace Game {
	GameState g_gameState = {};

	std::vector<Player> g_players;
	std::vector<Stalker> g_stalkers;

	void Init() {
		g_gameState = GameState::PLAYING;
		WeaponManager::Init();

		CreatePlayers();

		Stalker stalker1 = g_stalkers.emplace_back(glm::vec3(glm::vec3(-37.0f, 5.0f, 0.0f)));
	}

	void Update(double deltaTime) {
		UpdatePhysics();

		// Update enemies
		if (g_players[0].IsMoving()) {
			float enemySpeed = 20.0f;
			glm::vec3 stalkerPos = g_stalkers[0].GetPosition();
			glm::vec3 playerPos = g_players[0].getPosition();

			glm::vec3 moveDirection = glm::normalize(playerPos - stalkerPos);
			glm::vec3 newPosition = stalkerPos + moveDirection * enemySpeed * float(deltaTime);

			g_stalkers[0].SetPosition(glm::vec3(newPosition.x, stalkerPos.y, newPosition.z));

			float yawDegrees = glm::degrees(atan2(moveDirection.x, moveDirection.z));
			g_stalkers[0].SetRotationEuler(glm::vec3(0.0f, yawDegrees, 0.0f));
		}
		
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
		Animation* glockADSFire1Animation = AssetManager::GetAnimationByName("Glock_ADS_Fire");
		Animation* glockADSWalkAnimation = AssetManager::GetAnimationByName("Glock_ADS_Walk");

		Animator* p90Animator = AssetManager::GetAnimatorByName("P90Animator");
		

		Animation* p90ReloadAnimation = AssetManager::GetAnimationByName("P90_Reload");
		Animation* p90IdleAnimation = AssetManager::GetAnimationByName("P90_Idle");
		Animation* p90WalkAnimation = AssetManager::GetAnimationByName("P90_Walk");
		Animation* p90Fire0Animation = AssetManager::GetAnimationByName("P90_Fire0");
		Animation* p90DrawAnimation = AssetManager::GetAnimationByName("P90_Draw");

		Animator* aks74uAnimator = AssetManager::GetAnimatorByName("AKS74UAnimator");

		Animation* aks74uIdleAnimation = AssetManager::GetAnimationByName("AKS74U_Idle");
		Animation* aks74uReloadEmptyAnimation = AssetManager::GetAnimationByName("AKS74U_ReloadEmpty");
		Animation* aks74uDrawAnimation = AssetManager::GetAnimationByName("AKS74U_Draw");
		Animation* aks74uWalkAnimation = AssetManager::GetAnimationByName("AKS74U_Walk");
		Animation* aks74uADSInAnimation = AssetManager::GetAnimationByName("AKS74U_ADS_In");
		Animation* aks74uADSOutAnimation = AssetManager::GetAnimationByName("AKS74U_ADS_Out");
		Animation* aks74uADSIdleAnimation = AssetManager::GetAnimationByName("AKS74U_ADS_Idle");
		Animation* aks74uADSFireAnimation = AssetManager::GetAnimationByName("AKS74U_ADS_Fire");
		Animation* aks74uADSWalkAnimation = AssetManager::GetAnimationByName("AKS74U_ADS_Walk");

		Animation* katanaIdleAnimation = AssetManager::GetAnimationByName("Knife_Idle");
		Animation* katanaDrawAnimation = AssetManager::GetAnimationByName("Knife_Draw");

		Animator* katanaAnimator = AssetManager::GetAnimatorByName("KatanaAnimator");

		WeaponInfo* equipedWeapon = g_players[0].GetEquipedWeaponInfo();

		static std::string previousWeapon = equipedWeapon->name;
		static float drawAnimationFinishTime = 0.0f;
		static float ADSInAnimationFinishTime = 0.0f;
		static float ADSOutAnimationFinishTime = 0.0f;

		static bool isDrawing = false;
		static bool isInADS = false;

		//g_players[0].FireWeapon();

		g_players[0].UpdateWeaponLogic();

	
		if (!equipedWeapon) {
			std::cout << "ERROR: No equipped weapon!" << std::endl;
		}

		if (equipedWeapon->name == "Glock") {
			if (Keyboard::KeyJustPressed(GLFW_KEY_F)) {
				glockAnimator->PlayAnimation(glockDrawAnimation);
				drawAnimationFinishTime = 0.0f;
				isDrawing = true;
			}

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;

				
				if (drawAnimationFinishTime >= 0.65f) {
					glockAnimator->PlayAnimation(glockIdleAnimation);
					isDrawing = false; 
					g_players[0].SetWeaponAction(WeaponAction::IDLE);
				}
			}

			glockAnimator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "AKS74U") {
			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;


				if (drawAnimationFinishTime >= 0.65f) {
					aks74uAnimator->PlayAnimation(aks74uIdleAnimation);
					isDrawing = false;
				}
			}

			aks74uAnimator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "P90") {

			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;

			
				if (drawAnimationFinishTime >= 0.85f) {
					p90Animator->PlayAnimation(p90IdleAnimation);
					isDrawing = false;  
				}
			}

			p90Animator->UpdateAnimation(deltaTime);
		}
		else if (equipedWeapon->name == "Katana") {
			if (isDrawing) {
				drawAnimationFinishTime += deltaTime;


				if (drawAnimationFinishTime >= 0.85f) {
					katanaAnimator->PlayAnimation(katanaIdleAnimation, 0.85f);
					isDrawing = false;
				}
			}

			katanaAnimator->UpdateAnimation(deltaTime);
		}

		if (equipedWeapon->name == "Glock" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
			
		
				g_players[0].EquipWeapon("AKS74U");
				aks74uAnimator->PlayAnimation(aks74uDrawAnimation, 1.5);
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
			
			g_players[0].EquipWeapon("Katana");
			katanaAnimator->PlayAnimation(katanaDrawAnimation, 0.75f);
			AudioManager::PlayAudio(g_players[0].GetEquipedWeaponInfo()->audioFiles.draw, 1.0f, 1.0f);
			/*p90Animator->Reset();*/
			previousWeapon = equipedWeapon->name;
			drawAnimationFinishTime = 0.0f;
			isDrawing = true;
		}
		else if (equipedWeapon->name == "Katana" && Keyboard::KeyJustPressed(GLFW_KEY_1)) {
		
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
			glockAnimator->GetCurrentAnimation() == glockADSWalkAnimation ||
			glockAnimator->GetCurrentAnimation() == glockADSFire1Animation ||
			aks74uAnimator->GetCurrentAnimation() == aks74uADSInAnimation || 
			aks74uAnimator->GetCurrentAnimation() == aks74uADSIdleAnimation ||
			aks74uAnimator->GetCurrentAnimation() == aks74uADSOutAnimation ||
			aks74uAnimator->GetCurrentAnimation() == aks74uADSWalkAnimation ||
			aks74uAnimator->GetCurrentAnimation() == aks74uReloadEmptyAnimation
			) {
			UpdateWeaponPositionByName(equipedWeapon->name, true);
		}
		else {
			UpdateWeaponPositionByName(equipedWeapon->name);
		}
		
	}

	void UpdatePhysics() {
		for (GameObject& gameObject : Scene::GetGameObjects()) {
			if (gameObject.GetPhysicsId() != 0) {
				RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(gameObject.GetPhysicsId());

				if (rigidDynamic) {
					rigidDynamic->m_previousPosition = gameObject.GetPosition();
					rigidDynamic->m_previousRotation = gameObject.GetRotationMatrix();

					int smoothFactor = 10;

					gameObject.SetPosition(rigidDynamic->GetCurrentPosition());
					gameObject.SetRotationMatrix(rigidDynamic->GetCurrentRotationMatrix());
				}
			}
		}
	}

	void CreatePlayers() {
		Player player(glm::vec3(35.0f, 5.5f, 55.0f), 5.2f);
		player.EquipWeapon("Glock");
		player.InitWeaponStates();

		g_players.push_back(player);
	}

	Player& GetPLayerByIndex(int index) {
		if (index < 0 || index >= g_players.size()) {
			throw std::out_of_range("ERROR::GetPlayerByIndex::Index out of range!");
		}
		return g_players[index];
	}

	std::vector<Stalker>& GetAllStalkers() {
		return g_stalkers;
	}

	void UpdateWeaponPositionByName(std::string name, bool flipRotation) {
		Model* weaponModel = AssetManager::GetModelByName(name);

		glm::vec3 gunPosition;

		if (name == "Glock") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].m_camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].m_camera.cameraUp * -4.20f);    // Offset downward
		} else if (name == "AKS74U") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].m_camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].m_camera.cameraUp * -4.2f);    // Offset downward
		}
		else if (name == "P90") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].m_camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].m_camera.cameraUp * -4.2f);    // Offset downward
		}
		else if (name == "Katana") {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].m_camera.cameraFront * 0.7f) +   // Offset forward
				(g_players[0].m_camera.cameraUp * -3.7f);    // Offset downward
		}
		else {
			gunPosition = g_players[0].getPosition() +
				(g_players[0].m_camera.cameraFront * -0.7f) +   // Offset forward
				(g_players[0].m_camera.cameraUp * -3.8f);    // Offset downward
		}

		glm::mat4 gunRotation = glm::mat4(1.0f);

		float theta = acos(glm::dot(g_players[0].m_camera.worldUp, g_players[0].m_camera.cameraFront) /
			(glm::length(g_players[0].m_camera.worldUp) * glm::length(g_players[0].m_camera.cameraFront)));
		gunRotation = glm::rotate(gunRotation, glm::half_pi<float>() - theta, g_players[0].m_camera.cameraRight);

		glm::vec2 front2D = glm::vec2(g_players[0].m_camera.cameraFront.x, g_players[0].m_camera.cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), glm::normalize(front2D)));
		gunRotation = glm::rotate(gunRotation, g_players[0].m_camera.cameraFront.z < 0 ? theta : -theta, g_players[0].m_camera.worldUp);

		glm::mat4 localRotationFix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
		localRotationFix = glm::rotate(localRotationFix, glm::radians(5.0f), glm::vec3(1.9f, 0.0f, 0.0f)); 
		if (name == "P90" || name == "Katana" || flipRotation) {
			localRotationFix = glm::rotate(localRotationFix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		if (name == "Katana") {
		
			glm::mat4 katanaForwardTilt = glm::rotate(
				glm::mat4(1.0f),
				glm::radians(10.0f),
				glm::vec3(100.0f, -10.0f, -10.0f)
			);
			localRotationFix = katanaForwardTilt * localRotationFix;
		}
		gunRotation = gunRotation * localRotationFix;

		g_players[0].m_currentWeaponGameObject.SetPosition(gunPosition);
		g_players[0].m_currentWeaponGameObject.SetRotationMatrix(gunRotation);
	}

	GameState GetGameState() {
		return g_gameState;
	}

	void SetGameState(GameState newState) {
		g_gameState = newState;
	}
}