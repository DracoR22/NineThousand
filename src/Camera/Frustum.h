#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Core/Model.h"
#include "../Common/AABB.h"

struct FrustumPlane {
    glm::vec3 normal = { 0.f, 1.f, 0.f };
    float offset;
};

class Frustum {
private: 
    FrustumPlane m_planes[6];
    glm::vec3 m_corners[8];
    glm::vec3 m_boundsMin = glm::vec3(0);
    glm::vec3 m_boundsMax = glm::vec3(0);
public:
    void Update(const glm::mat4 projectionView);
    glm::vec3 IntersectPlanes(const glm::vec3& n1, float d1, const glm::vec3& n2, float d2, const glm::vec3& n3, float d3);
    float SignedDistance(const glm::vec3& point, const FrustumPlane& plane) const;
    bool IntersectsAABB(const AABB& aabb);
    bool IntersectsAABBFast(const AABB& aabb);

    std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
};



   