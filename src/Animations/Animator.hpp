#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Bone.hpp"
#include "Animation.hpp"

class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_DeltaTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt)
	{
		if (!m_CurrentAnimation) return;

		m_DeltaTime = dt;
		float prevTime = m_CurrentTime;

		// Advance time
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt * m_AnimationSpeed;

		// Detect loop (animation finished)
		if (m_CurrentTime >= m_CurrentAnimation->GetDuration() - 0.0001f) {
			m_AnimationFinished = true;
			m_CurrentTime = 0.0f; // Reset for next cycle
		}

		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}

	void PlayAnimation(Animation* pAnimation, float speed = 1.0f)
	{
	     	m_AnimationFinished = false;
			m_CurrentAnimation = pAnimation;
			m_CurrentTime = 0.0f;
			m_AnimationSpeed = speed;
	}

	bool IsAnimationFinished() const {
		return m_AnimationFinished;
	}

	void Reset()
	{
		m_CurrentTime = 0.0f;
		m_AnimationFinished = false;
		m_AnimationSpeed = 1.0f;
		m_CurrentAnimation = nullptr; 

		// Reset the bone matrices to identity
		for (auto& matrix : m_FinalBoneMatrices) {
			matrix = glm::mat4(1.0f);
		}
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

	Animation* GetCurrentAnimation() const {
		return m_CurrentAnimation;
	}

private:
	float m_CurrentTime;
	float m_DeltaTime;
	float m_AnimationSpeed = 1.0f;

	bool m_AnimationFinished = false;

	std::vector<glm::mat4> m_FinalBoneMatrices;
private:
	Animation* m_CurrentAnimation;
};