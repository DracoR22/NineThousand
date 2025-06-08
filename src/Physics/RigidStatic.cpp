#include "RigidStatic.h"

RigidStatic::~RigidStatic() {
	if (m_pxRigidStatic) {
		m_pxRigidStatic->release();
		m_pxRigidStatic = nullptr;
	}
}

void RigidStatic::SetPxRigidStatic(PxRigidStatic* rigidStatic) {
	m_pxRigidStatic = rigidStatic;
}

PxRigidStatic* RigidStatic::GetPxRigidStatic() {
	return m_pxRigidStatic;
}

glm::vec3 RigidStatic::GetCurrentPosition() {
	if (m_pxRigidStatic) {
		PxTransform transform = m_pxRigidStatic->getGlobalPose();
		return glm::vec3(transform.p.x, transform.p.y, transform.p.z);
	}
	return glm::vec3(0.0f);
}

glm::quat RigidStatic::GetCurrentRotation() {
	if (m_pxRigidStatic) {
		PxTransform transform = m_pxRigidStatic->getGlobalPose();
		return glm::quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	}
	return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}
