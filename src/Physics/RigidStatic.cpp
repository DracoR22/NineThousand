#include "RigidStatic.h"

void RigidStatic::SetPxRigidStatic(PxRigidStatic* rigidStatic, PxShape* shape) {
	m_pxRigidStatic = rigidStatic;
	m_pxShape = shape;
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
