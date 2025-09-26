#include "./Mannequin.h"

#include "../Common/Enums.hpp"
#include "./Game.h"
#include "../Audio/AudioManager.h"

Mannequin::Mannequin(glm::vec3 position) {
	m_position = position;
	m_rotationEuler = glm::vec3(1.0f, 90.0f, 1.0f);
	m_size = glm::vec3(0.042f);

	m_health = 100;

	uint64_t objectId = Utils::GenerateUniqueID();

	uint64_t controllerPhysicsId = Physics::CreateCharacterController(objectId, position, 10.0f, ObjectType::MANNEQUIN);
	m_controllerPhysicsId = controllerPhysicsId;

	m_objectId = objectId;
	m_chasedPlayerId = -1;

	m_state = MannequinState::REST;

	uint64_t ragdollId = Physics::CreateMannequinRagdoll();
	m_ragdollId = ragdollId;

	Ragdoll* ragdoll = Physics::GetRagdollById(m_ragdollId);
	if (ragdoll) {
		ragdoll->SetUserData(m_objectId, ObjectType::MANNEQUIN);
	}
}

void Mannequin::Update(double deltaTime) {
	Animator* mannequinAnimator = AssetManager::GetAnimatorByName("MannequinAnimator");
	mannequinAnimator->UpdateAnimation(deltaTime);
	UpdateMovement();

	if (m_health <= 0 && m_state != MannequinState::DEAD) {
		Kill();
	}

	if (m_state == MannequinState::CHASE) {
		if (mannequinAnimator->IsAnimationFinished()) {
			mannequinAnimator->PlayAnimation(AssetManager::GetAnimationByName("Mannequin_Run"));
		}

		// move the manequinn
		float speed = 20.0f;
		Player* player = Game::GetPlayerById(m_chasedPlayerId);
		glm::vec3 playerPos = player->getPosition();

		glm::vec3 moveDirection = glm::normalize(playerPos - m_position);
		Physics::MoveCharacterController(m_controllerPhysicsId, moveDirection * speed * static_cast<float>(deltaTime));

		float yawDegrees = glm::degrees(atan2(moveDirection.x, moveDirection.z));
		SetRotationEuler(glm::vec3(0.0f, yawDegrees, 0.0f));

		m_isMoving = true;
	}

	UpdateAudio(deltaTime);
}

void Mannequin::UpdateMovement() {
	Animator* mannequinAnimator = AssetManager::GetAnimatorByName("MannequinAnimator");
	Ragdoll* ragdoll = Physics::GetRagdollById(m_ragdollId);

	if (m_state != MannequinState::DEAD || m_state == MannequinState::DEAD) {
		glm::vec3 characterControllerPos = Physics::GetCharacterControllerPosition(m_controllerPhysicsId);
		m_position = glm::vec3(characterControllerPos.x, characterControllerPos.y / m_height, characterControllerPos.z);
	
		ragdoll->SetGlobalPoseFromAnimator(mannequinAnimator, GetModelMatrix());
	}
	/*else if (m_state == MannequinState::DEAD) {
		Ragdoll* ragdoll = Physics::GetRagdollById(m_ragdollId);

		if (ragdoll) {
			for (RigidComponent& rigidComponent : ragdoll->GetRigidComponents()) {
				RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(rigidComponent.id);

				if (rigidDynamic) {
					glm::vec3 pos = rigidDynamic->GetCurrentPosition();
					glm::quat rot = rigidDynamic->GetCurrentRotation();

					glm::mat4 boneWorldMatrix = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot);
					mannequinAnimator->OverrideBoneTransform(rigidComponent.boneName, boneWorldMatrix, GetModelMatrix());
				}
			}
		}
	}*/
}

void Mannequin::UpdateAudio(double deltaTime) {
	const std::vector<const char*> sloshFootSteps = {
	   "slosh1.wav",
	   "slosh2.wav",
	   "slosh3.wav",
		"slosh4.wav"
	};

	static float m_footstepTimer = 0.0f;
	static float m_footstepInterval = 0.25f;
	static bool m_isOnWaterSurface = true;

	if (m_isMoving && m_isOnWaterSurface) {
		m_footstepTimer -= static_cast<float>(deltaTime);

		if (m_footstepTimer <= 0.0f) {
			int randAudio = std::rand() % sloshFootSteps.size();
			AudioManager::PlayAudio(sloshFootSteps[randAudio], 0.6f, 1.0f);

			m_footstepTimer = m_footstepInterval;
		}
	}
	else {
		m_footstepTimer = 0.0f;
	}
}

void Mannequin::SetPosition(glm::vec3 position) {
	m_position = position;
}

void Mannequin::SetRotationEuler(glm::vec3 rotation) {
	m_rotationEuler = rotation;
}

glm::mat4 Mannequin::GetModelMatrix() const {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, m_position);
	model = model * glm::toMat4(glm::quat(glm::radians(m_rotationEuler)));;
	model = model * glm::scale(glm::mat4(1.0f), m_size);

	return model;
}

void Mannequin::TakeDamage(int damageAmount, uint64_t playerId) {
	if (m_health > 0) {
		m_health -= damageAmount;
	}

	AudioManager::PlayAudio("Flesh_Bloody_Break.wav", 1.0f, 1.0f);

	if (m_state != MannequinState::CHASE && m_state != MannequinState::DEAD) {
		ChasePlayer(playerId);
	}
}

void Mannequin::Kill() {
	Animator* mannequinAnimator = AssetManager::GetAnimatorByName("MannequinAnimator");

	m_state = MannequinState::DEAD;
	m_isMoving = false;
	mannequinAnimator->PlayAnimation(AssetManager::GetAnimationByName("Mannequin_Death"));

	//Ragdoll* ragdoll = Physics::GetRagdollById(m_ragdollId);
	//ragdoll->ActivatePhysics();
}

void Mannequin::ChasePlayer(uint64_t playerId) {
	m_state = MannequinState::CHASE;
	m_chasedPlayerId = playerId;
}
