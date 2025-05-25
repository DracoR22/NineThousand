#pragma once

#include <random>
#include <glm/glm.hpp>

namespace Utils {
	void AverageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions);
	float RandomFloat(float min, float max);
}