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

	std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
		const auto inv = glm::inverse(proj * view);

		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					const glm::vec4 pt =
						inv * glm::vec4(
							2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}

		return frustumCorners;
	}

	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane, float windowWidth, float windowHeight, float fov, glm::mat4 viewMatrix) {
		glm::mat4 cascadeProj = glm::perspective(
			glm::radians(fov),
			windowWidth / windowHeight,
			nearPlane,
			farPlane
		);
		const auto corners = GetFrustumCornersWorldSpace(cascadeProj, viewMatrix);

		glm::vec3 center = glm::vec3(0, 0, 0);
		for (const auto& v : corners)
		{
			center += glm::vec3(v);
		}
		center /= corners.size();

		glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50.0f, 20.0f));

		const int shadowMapResolution = 1024;

		const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();
		for (const auto& corner : corners)
		{
			const auto trf = lightView * corner;
			minX = std::min(minX, trf.x);
			maxX = std::max(maxX, trf.x);
			minY = std::min(minY, trf.y);
			maxY = std::max(maxY, trf.y);
			minZ = std::min(minZ, trf.z);
			maxZ = std::max(maxZ, trf.z);
		}

		float width = maxX - minX;
		float height = maxY - minY;
		float texelSizeX = width / float(shadowMapResolution);
		float texelSizeY = height / float(shadowMapResolution);

		float snapMinX = std::floor(minX / texelSizeX) * texelSizeX;
		float snapMaxX = snapMinX + std::ceil(width / texelSizeX) * texelSizeX;
		float snapMinY = std::floor(minY / texelSizeY) * texelSizeY;
		float snapMaxY = snapMinY + std::ceil(height / texelSizeY) * texelSizeY;

		constexpr float zMult = 10.0f;
		float zNear = (minZ < 0 ? minZ * zMult : minZ / zMult);
		float zFar = (maxZ < 0 ? maxZ / zMult : maxZ * zMult);

		const glm::mat4 lightProjection = glm::ortho(snapMinX, snapMaxX, snapMinY, snapMaxY, zNear, zFar);
		return lightProjection * lightView;
	}

	std::vector<glm::mat4> GetLightSpaceMatrices(const float nearPlane, const float farPlane, std::vector<float>& shadowCascadeLevels, float windowWidth, float windowHeight, float fov, glm::mat4 viewMatrix)
	{
		std::vector<glm::mat4> ret;
		for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
		{
			if (i == 0)
			{
				ret.push_back(GetLightSpaceMatrix(nearPlane, shadowCascadeLevels[i], windowWidth, windowHeight, fov, viewMatrix));
			}
			else if (i < shadowCascadeLevels.size())
			{
				ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i], windowWidth, windowHeight, fov, viewMatrix));
			}
			else
			{
				ret.push_back(GetLightSpaceMatrix(shadowCascadeLevels[i - 1], farPlane, windowWidth, windowHeight, fov, viewMatrix));
			}
		}
		return ret;
	}
}