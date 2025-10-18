#pragma once

#include <glm/glm.hpp>
#include "../PathFinding/PathFinding.h"
#include "../Physics/Physics.h"
#include "../Core/AssetManager.h"

enum MannequinState {
	REST,
	CHASE,
	DEAD
};

class Mannequin {
public:
	Mannequin() = default;
	Mannequin(glm::vec3 position);

	void Update(double deltaTime);
	void UpdateMovement();
	void UpdateAudio(double deltaTime);

	void SetPosition(glm::vec3 position);
	void SetRotationEuler(glm::vec3 rotation);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetRotationEuler() const { return m_rotationEuler; };
	glm::mat4 GetModelMatrix() const;

	uint64_t GetObjectId() const { return m_objectId; };
	float GetHeight() const { return m_height; };

	void TakeDamage(int damageAmount, uint64_t playerId);
	void ChasePlayer(uint64_t playerId);
	void Kill();

	PathFinding m_pathFinder;
private:
	glm::vec3 m_position;
	glm::vec3 m_rotationEuler;
	glm::vec3 m_size;

	int m_health;
	bool m_isMoving = false;
	float m_height = 15.0f;

	uint64_t m_controllerPhysicsId;
	uint64_t m_ragdollId;

	uint64_t m_objectId;
	uint64_t m_chasedPlayerId;

	MannequinState m_state;
	bool m_isReleased = false;
};