#include "Game.h"

namespace Game {
	GameState g_gameState = {};

	std::vector<Player> g_players;

	void Init() {
		g_gameState = GameState::PLAYING;
		WeaponManager::Init();

		CreatePlayers();

		Scene::AddMannequin(glm::vec3(-5.0f, 1.2f, 37.0f));
	}

	void Update(double deltaTime) {
		UpdatePhysics();
		g_players[0].Update(Window::GetDeltaTime());
		
		for (Mannequin& mannequin : Scene::GetAllMannequins()) {
			mannequin.Update(Window::GetDeltaTime());
		}

		ProcessBullets();

		std::vector<BloodSplatterObject>& bloodSplatterObjects = Scene::GetBloodSplatterObjects();
		for (BloodSplatterObject& bloodSplatterObject : Scene::GetBloodSplatterObjects()) {
			bloodSplatterObject.Update(Window::GetDeltaTime());
		}

		const float MAX_LIFETIME = 1.0f; 
		bloodSplatterObjects.erase(std::remove_if(bloodSplatterObjects.begin(), bloodSplatterObjects.end(),[MAX_LIFETIME](const BloodSplatterObject& obj) {
			return obj.GetLifeTime() > MAX_LIFETIME;
		  }
		), bloodSplatterObjects.end());

		for (PickUpObject& pickUpObject : Scene::GetPickUpObjects()) {
			pickUpObject.Update(Window::GetDeltaTime());
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
	
		if (!equipedWeapon) {
			std::cout << "ERROR: No equipped weapon!" << std::endl;
		}

		if (equipedWeapon->name == "Glock") {
			if (Keyboard::KeyJustPressed(GLFW_KEY_F)) {
				glockAnimator->PlayAnimation(AssetManager::GetAnimationByName("Glock_Spawn"));
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
		
		Physics::Simulate(Window::GetDeltaTime());
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

		for (BulletCaseObject& bulletCase : Scene::GetBulletCaseObjects()) {
			if (bulletCase.GetPhysicsId() != 0) {
				RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(bulletCase.GetPhysicsId());
				if (rigidDynamic) {
					rigidDynamic->m_previousPosition = bulletCase.GetPosition();
					rigidDynamic->m_previousRotation = bulletCase.GetRotationMatrix();

					int smoothFactor = 10;
					bulletCase.SetPosition(rigidDynamic->GetCurrentPosition());
					bulletCase.SetRotationMatrix(rigidDynamic->GetCurrentRotationMatrix());
				}
			}
		}
	}

	void CreatePlayers() {
		Player player(glm::vec3(35.0f, 5.5f, 55.0f), 4.2f);
		player.EquipWeapon("Glock");
		player.InitWeaponStates();

		g_players.push_back(player);
	}

	Player* GetPlayerById(uint64_t playerId) {
		for (Player& player : g_players) {
			if (player.GetPlayerId() == playerId) {
				return &player;
			}
		}

		return nullptr;
	}

	Player& GetPLayerByIndex(int index) {
		if (index < 0 || index >= g_players.size()) {
			throw std::out_of_range("ERROR::GetPlayerByIndex::Index out of range!");
		}
		return g_players[index];
	}

	void ProcessBullets() {
		Player& player = g_players[0];
		int weaponDamage = 25;

		if (player.m_firedThisFrame) {
			player.m_firedThisFrame = false;

			glm::vec3 playerPos = player.m_camera.cameraPos;
			glm::vec3 cameraDir = glm::normalize(player.m_camera.cameraFront);
			float maxDistance = 100000.0f;  // Maximum bullet range

			PhysicsRayResult rayResult = Physics::CastPhysXRay(playerPos, cameraDir, maxDistance);

			if (rayResult.hitFound) {
				PhysicsType& physicsType = rayResult.userData.physicsType;
				ObjectType& objectType = rayResult.userData.objectType;
				uint64_t physicsId = rayResult.userData.physicsId;
				uint64_t objectId = rayResult.userData.objectId;

				if (physicsType == PhysicsType::RIGID_DYNAMIC) {
					float strength = 5000.0f;
					
					Physics::AddForceToRigidDynamic(physicsId, cameraDir, strength);
				}

				if (objectType == ObjectType::MANNEQUIN) {
					Mannequin* mannequin = Scene::GetMannequinById(objectId);
					if (mannequin) {
						mannequin->TakeDamage(weaponDamage, player.GetPlayerId());
					}

					Scene::AddBloodSplatterObject(rayResult.hitPosition, -rayResult.rayDirection);
				}
			}
		}
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

		double mouseDX = Mouse::getDX();
		double mouseDY = Mouse::getDY();

		float swayAmount = 0.002f;
		static glm::vec3 weaponSwayOffset = glm::vec3(0.0f);

		glm::vec3 targetSwayOffset =
			g_players[0].m_camera.cameraRight * (-(float)mouseDX * swayAmount) +
			g_players[0].m_camera.cameraUp * (-(float)mouseDY * swayAmount);

		float smoothSpeed = 5.0f; // Increase for snappier, decrease for floatier
		weaponSwayOffset = glm::mix(weaponSwayOffset, targetSwayOffset, smoothSpeed * Window::GetDeltaTime());

		// Apply
		gunPosition += weaponSwayOffset;


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