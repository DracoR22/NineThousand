#pragma once

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <mutex>

namespace Utils {
	void AverageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions);
	float RandomFloat(float min, float max);
	uint64_t GenerateUniqueID();

	// glm
	glm::quat GlmVec3ToGlmQuat(glm::vec3 data);
}