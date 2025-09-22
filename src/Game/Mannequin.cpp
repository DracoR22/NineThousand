#include "./Mannequin.h"

#include "../Common/Enums.hpp"
#include "./Game.h"
#include "../Audio/AudioManager.h"

Mannequin::Mannequin(glm::vec3 position) {
	m_position = position;
	m_rotationEuler = glm::vec3(1.0f, 90.0f, 1.0f);
	m_health = 100;

	uint64_t objectId = Utils::GenerateUniqueID();

	uint64_t physicsId = Physics::CreateCharacterController(objectId, position, 10.0f, ObjectType::MANNEQUIN);
	m_physicsId = physicsId;
	m_objectId = objectId;
	m_chasedPlayerId = -1;

	m_state = MannequinState::REST;
}

void Mannequin::Update(double deltaTime) {
	if (m_health <= 0) return;

	Animator* mannequinAnimator = AssetManager::GetAnimatorByName("MannequinAnimator");
	mannequinAnimator->UpdateAnimation(deltaTime);
	UpdateMovement();

	if (m_state == MannequinState::CHASE) {
		if (mannequinAnimator->IsAnimationFinished()) {
			mannequinAnimator->PlayAnimation(AssetManager::GetAnimationByName("Mannequin_Run"));
		}

		// move the manequinn
		float speed = 20.0f;
		Player* player = Game::GetPlayerById(m_chasedPlayerId);
		glm::vec3 playerPos = player->getPosition();

		glm::vec3 moveDirection = glm::normalize(playerPos - m_position);
		Physics::MoveCharacterController(m_physicsId, moveDirection * speed * static_cast<float>(deltaTime));

		float yawDegrees = glm::degrees(atan2(moveDirection.x, moveDirection.z));
		SetRotationEuler(glm::vec3(0.0f, yawDegrees, 0.0f));
	}
}

void Mannequin::UpdateMovement() {
	glm::vec3 characterControllerPos = Physics::GetCharacterControllerPosition(m_physicsId);
	m_position = characterControllerPos;
}

void Mannequin::SetPosition(glm::vec3 position) {
	m_position = position;
}

void Mannequin::SetRotationEuler(glm::vec3 rotation) {
	m_rotationEuler = rotation;
}

void Mannequin::TakeDamage(int damageAmount, uint64_t playerId) {
	m_health -= damageAmount;

	AudioManager::PlayAudio("Flesh_Bloody_Break.wav", 1.0f, 1.0f);

	if (m_health <= 0) {
		m_state = MannequinState::DEAD;
	}

	if (m_state != MannequinState::CHASE) {
		ChasePlayer(playerId);
	}
}

void Mannequin::ChasePlayer(uint64_t playerId) {
	m_state = MannequinState::CHASE;
	m_chasedPlayerId = playerId;
}
