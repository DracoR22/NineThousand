#include "Utils.h"

namespace Utils {
	uint64_t g_uniqueId = 1;

	void AverageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions) {
		if (!existingContributions) {
			baseVec = addition;
		}
		else {
			float f = 1 / ((float)existingContributions + 1);
			baseVec *= (float)(existingContributions)*f;
			baseVec += addition * f;
		}
	}

	float RandomFloat(float min, float max) {
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	uint64_t GenerateUniqueID() {
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		return g_uniqueId++;
	}

	glm::quat GlmVec3ToGlmQuat(glm::vec3 data) {
		return glm::quat(glm::radians(data));
	}
}