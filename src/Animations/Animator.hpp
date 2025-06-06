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
		if (!m_CurrentAnimation) {
			std::cerr << "Error: No animation set in Animator!" << std::endl;
			return;
		}

		m_DeltaTime = dt;
		float prevTime = m_CurrentTime;

		float ticksPerSecond = m_CurrentAnimation->GetTicksPerSecond();
		float duration = m_CurrentAnimation->GetDuration() / ticksPerSecond;

		m_CurrentTime += dt * m_AnimationSpeed;

		// Detect loop (animation finished)
		if (m_CurrentTime >= duration) {
			m_CurrentTime = duration; 
			m_AnimationFinished = true;
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

	int GetAnimationFrameNumber() {
		return m_CurrentTime * m_CurrentAnimation->GetTicksPerSecond();
	}

	bool AnimationIsPastFrameNumber(int frameNumber) {
		return frameNumber < GetAnimationFrameNumber();
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

	float GetCurrentTime() const {
		return m_CurrentTime;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		const std::string& nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		std::shared_ptr<Bone> Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone) {
			Bone->Update(m_CurrentTime * m_CurrentAnimation->GetTicksPerSecond());
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		const auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			int index = boneInfoMap.at(nodeName).id;
			m_FinalBoneMatrices[index] = globalTransformation * boneInfoMap.at(nodeName).offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices() {
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
	Animation* m_CurrentAnimation;
};