#include "AABB.h"

AABB::AABB(glm::vec3& min, glm::vec3& max) {
	center = (max + min) * 0.5f;
	extents = glm::vec3(max.x - center.x, max.y - center.y, max.z - center.z);
	boundsMin = min;
	boundsMax = max;
}

AABB AABB::TransformModelToWorldMatrix(const glm::mat4& matrix) const {
    glm::vec3 corners[8] = {
        glm::vec3(boundsMin.x, boundsMin.y, boundsMin.z),
        glm::vec3(boundsMax.x, boundsMin.y, boundsMin.z),
        glm::vec3(boundsMin.x, boundsMax.y, boundsMin.z),
        glm::vec3(boundsMax.x, boundsMax.y, boundsMin.z),
        glm::vec3(boundsMin.x, boundsMin.y, boundsMax.z),
        glm::vec3(boundsMax.x, boundsMin.y, boundsMax.z),
        glm::vec3(boundsMin.x, boundsMax.y, boundsMax.z),
        glm::vec3(boundsMax.x, boundsMax.y, boundsMax.z)
    };

    glm::vec3 newMin(FLT_MAX);
    glm::vec3 newMax(-FLT_MAX);

    for (int i = 0; i < 8; ++i) {
        glm::vec3 transformed = glm::vec3(matrix * glm::vec4(corners[i], 1.0f));
        newMin = glm::min(newMin, transformed);
        newMax = glm::max(newMax, transformed);
    }

    return AABB(newMin, newMax);
}