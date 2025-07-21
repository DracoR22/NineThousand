#include "./Stalker.h"

Stalker::Stalker(glm::vec3 position): 
  m_position(position), m_rotationEuler(1.0f) {}

void Stalker::SetPosition(glm::vec3 position) {
	m_position = position;
}

void Stalker::SetRotationEuler(glm::vec3 rotation) {
	m_rotationEuler = rotation;
}