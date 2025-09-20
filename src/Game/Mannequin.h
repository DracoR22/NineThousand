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

	void SetPosition(glm::vec3 position);
	void SetRotationEuler(glm::vec3 rotation);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetRotationEuler() const { return m_rotationEuler; };
	uint64_t GetObjectId() const { return m_objectId; };
	float GetHeight() const { return m_height; };

	void TakeDamage(int damageAmount, uint64_t playerId);
	void ChasePlayer(uint64_t playerId);

	PathFinding m_pathFinder;
private:
	glm::vec3 m_position;
	glm::vec3 m_rotationEuler;

	int m_health;
	float m_height = 15.0f;

	uint64_t m_physicsId;
	uint64_t m_objectId;
	uint64_t m_chasedPlayerId;

	MannequinState m_state;
};