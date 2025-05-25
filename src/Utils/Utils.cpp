#include "Utils.h"

namespace Utils {
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
}