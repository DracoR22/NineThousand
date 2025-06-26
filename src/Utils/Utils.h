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

	// renderer
	std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane, float windowWidth, float windowHeight, float fov, glm::mat4& viewMatrix);
	std::vector<glm::mat4> GetLightSpaceMatrices(const float nearPlane, const float farPlane, std::vector<float>& shadowCascadeLevels, float windowWidth, float windowHeight, float fov, glm::mat4& viewMatrix);
}