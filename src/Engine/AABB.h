#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class Frustum;
struct Plane;

struct MeshAABB
{
    glm::vec3 min = {0,0,0};
    glm::vec3 max = {0,0,0};
    glm::vec3 center = {0,0,0};
    glm::vec3 size = {0,0,0}; 

    MeshAABB() = default;

    MeshAABB(const glm::vec3& min, const glm::vec3& max)
        : center{ (max + min) * 0.5f }, size{ max.x - center.x, max.y - center.y, max.z - center.z }//NOTE(darius) a u shue?
    {}

    MeshAABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
        : center{ inCenter }, size{ iI, iJ, iK }
    {}

    std::array<glm::vec3, 8> getCorners();

    bool isOnOrForwardPlane(const Plane& plane);

    bool isOnFrustum(Frustum& camFrustum);

    bool isInside(const glm::vec3& point);
};
