#pragma once

#include <string>
#include <vector>

#include "../Animation/Animator.hpp"
#include "../Common/Enums.hpp"
#include "../Common/Types.hpp"

struct RigidComponent {
	uint64_t id;
	std::string boneName;
	float mass;
};

class Ragdoll {
public:
	std::string GetName() { return m_name; };

	void ActivatePhysics();
	void SetGlobalPoseFromAnimator(Animator* animator, glm::mat4 modelMatrix);

	std::vector<RigidComponent>& GetRigidComponents() { return m_rigidComponents; };

	void SetRigidDynamicId(uint64_t id);
	void SetRigidComponent(RigidComponent& component);

	void SetUserData(uint64_t objectId, ObjectType objectType);
private:
	std::string m_name;
	std::vector<uint64_t> m_rigidDynamicIds;
	std::vector<RigidComponent> m_rigidComponents;
};