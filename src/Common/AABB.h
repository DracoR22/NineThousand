#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class AABB {
public:
    AABB() = default;
    AABB(glm::vec3& min, glm::vec3& max);

    AABB TransformModelToWorldMatrix(const glm::mat4& matrix) const;

    const glm::vec3 GetBoundsMin() const { return boundsMin; }
    const glm::vec3 GetBoundsMax() const { return boundsMax; }

private:
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 extents = glm::vec3(0.0f);
    glm::vec3 boundsMin = glm::vec3(1e30f);
    glm::vec3 boundsMax = glm::vec3(-1e30f);
};