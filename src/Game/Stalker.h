#pragma once

#include <glm/glm.hpp>
#include "../PathFinding/PathFinding.h"

class Stalker {
public:
	Stalker() = default;
	Stalker(glm::vec3 position);

	void SetPosition(glm::vec3 position);
	void SetRotationEuler(glm::vec3 rotation);

	glm::vec3 GetPosition() const { return m_position; };
	glm::vec3 GetRotationEuler() const { return m_rotationEuler; };

	PathFinding m_pathFinder;
private:
	glm::vec3 m_position;
	glm::vec3 m_rotationEuler;
};