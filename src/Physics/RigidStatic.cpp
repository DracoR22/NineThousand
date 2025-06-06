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
