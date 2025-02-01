#pragma once
#include "HellTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <vector>

struct Plane {
    glm::vec3 normal;
    float offset;
};

struct Frustum {

public:
    void Update(const glm::mat4& projectionView);
    bool IntersectsAABB(const AABB& aabb);
    bool IntersectsAABB(const RenderItem& renderItem);
    bool IntersectsAABBFast(const AABB& aabb);
    bool IntersectsAABBFast(const RenderItem& renderItem);
    //bool IntersectsSphere(const Sphere& sphere);
    bool IntersectsPoint(const glm::vec3 point);
    std::vector<glm::vec3> GetFrustumCorners();

private:
    Plane CreatePlane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
    float SignedDistance(const glm::vec3& point, const Plane& plane) const;
    static glm::vec3 IntersectPlanes(const glm::vec3& n1, float d1, const glm::vec3& n2, float d2, const glm::vec3& n3, float d3);

    Plane m_planes[6];
};

