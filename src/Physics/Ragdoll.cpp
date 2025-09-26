#include "./Ragdoll.h"

#include "./RigidDynamic.h"
#include "./Physics.h"

void Ragdoll::ActivatePhysics() {
	for (uint64_t id : m_rigidDynamicIds) {
		RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(id);

		if (rigidDynamic) {
			PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();
			if (pxRigidDynamic) {
				pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
				pxRigidDynamic->wakeUp();
			}
		}
	}
}

void Ragdoll::SetGlobalPoseFromAnimator(Animator* animator, glm::mat4 modelMatrix) {
	if (!animator) return;

	std::map<std::string, BoneInfo> boneIDMap = animator->GetCurrentAnimation()->GetBoneIDMap();

	for (RigidComponent& rigidComponent : m_rigidComponents) {
		std::string targetBoneName = rigidComponent.boneName;

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

		RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(rigidComponent.id);
		if (rigidDynamic) {
			PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();
			if (pxRigidDynamic) {
				pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				pxRigidDynamic->setKinematicTarget(pxTransform);
			}
		}
	}
}

void Ragdoll::SetRigidComponent(RigidComponent& component) {
	m_rigidComponents.push_back(component);
}

void Ragdoll::SetRigidDynamicId(uint64_t id) {
	m_rigidDynamicIds.push_back(id);
}

void Ragdoll::SetUserData(uint64_t objectId, ObjectType objectType) {
	for (uint64_t id : m_rigidDynamicIds) {
		RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(id);

		if (rigidDynamic) {
			PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();

			if (pxRigidDynamic) {
				PhysicsUserData userData;
				userData.objectId = objectId;
				userData.objectType = objectType;
				userData.physicsType = PhysicsType::RIGID_DYNAMIC;
				userData.physicsId = id;
				pxRigidDynamic->userData = new PhysicsUserData(userData);
			}
		}
	}
}