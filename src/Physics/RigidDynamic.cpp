#include "RigidDynamic.h"

RigidDynamic::~RigidDynamic() {
	if (m_pxRigidDynamic) {
		m_pxRigidDynamic->release();
		m_pxRigidDynamic = nullptr;
	}
}

void RigidDynamic::SetPxRigidDynamic(PxRigidDynamic* rigidDynamic) {
	m_pxRigidDynamic = rigidDynamic;
}

void RigidDynamic::UpdateMassAndInertia(float mass) {
	PxRigidBodyExt::updateMassAndInertia(*m_pxRigidDynamic, mass);
}

glm::vec3 RigidDynamic::GetCurrentPosition() {
	if (m_pxRigidDynamic) {
		PxTransform transform = m_pxRigidDynamic->getGlobalPose();
		return glm::vec3(transform.p.x, transform.p.y, transform.p.z);
	}
	return glm::vec3(0.0f);
}

glm::quat RigidDynamic::GetCurrentRotation() {
	if (m_pxRigidDynamic) {
		PxTransform transform = m_pxRigidDynamic->getGlobalPose();
		return glm::quat(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	}
	return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

PxRigidDynamic* RigidDynamic::GetPxRigidDynamic() {
	return m_pxRigidDynamic;
}
