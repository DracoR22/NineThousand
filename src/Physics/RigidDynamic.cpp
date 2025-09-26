#include "RigidDynamic.h"
#include <glm/gtx/quaternion.hpp>

//RigidDynamic::~RigidDynamic() {
//	if (m_pxRigidDynamic) {
//		m_pxRigidDynamic->release();
//		m_pxRigidDynamic = nullptr;
//	}
//}

void RigidDynamic::SetPxRigidDynamic(PxRigidDynamic* rigidDynamic) {
	m_pxRigidDynamic = rigidDynamic;
}

void RigidDynamic::UpdateMassAndInertia(float mass) {
	PxRigidBodyExt::updateMassAndInertia(*m_pxRigidDynamic, mass);
}

void RigidDynamic::ActivatePhysics() {
	if (m_pxRigidDynamic) {
		m_pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
		m_pxRigidDynamic->wakeUp();
	}
}

void RigidDynamic::SetGlobalPoseFromTransform(PhysicsTransformData& transform) {
	PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
	PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
	PxTransform pxTransform(pxPos, pxRot);

	if (m_pxRigidDynamic) {
		m_pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		m_pxRigidDynamic->setKinematicTarget(pxTransform);
	}
}

void RigidDynamic::SetGlobalPoseFromAnimator(Animator* animator, glm::mat4 modelMatrix) {
	if (!animator || !m_pxRigidDynamic) return;

	std::string targetBoneName = "mixamorig1:Head";
	std::map<std::string, BoneInfo> boneIDMap = animator->GetCurrentAnimation()->GetBoneIDMap();

	if (boneIDMap.find(targetBoneName) == boneIDMap.end()) {
		std::cerr << "Bone " << targetBoneName << " not found!\n";
		return;
	}

	int boneIndex = boneIDMap[targetBoneName].id;

	glm::mat4 boneMatrix = animator->GetGlobalBoneMatrices()[boneIndex];

	glm::mat4 boneWorldMatrix = modelMatrix * boneMatrix;

	glm::vec3 position = glm::vec3(boneWorldMatrix[3]);
	glm::quat rotation = glm::quat_cast(boneWorldMatrix); 

	PxVec3 pxPos(position.x, position.y, position.z);
	PxQuat pxRot(rotation.x, rotation.y, rotation.z, rotation.w);
	PxTransform pxTransform(pxPos, pxRot);

	m_pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	m_pxRigidDynamic->setKinematicTarget(pxTransform);
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

glm::mat4 RigidDynamic::GetCurrentRotationMatrix() {
	if (m_pxRigidDynamic) {
		PxQuat q = m_pxRigidDynamic->getGlobalPose().q;
		return glm::toMat4(glm::quat(q.w, q.x, q.y, q.z)); // note: glm::quat(w, x, y, z)
	}
	return glm::mat4(1.0f);
}

PxRigidDynamic* RigidDynamic::GetPxRigidDynamic() {
	return m_pxRigidDynamic;
}

void RigidDynamic::MarkForRemoval() {
	m_markedForRemoval = true;
}
