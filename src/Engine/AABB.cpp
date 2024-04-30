#include <Engine/AABB.h>
#include <Engine/FrustumCulling.h>

std::array<glm::vec3, 8> MeshAABB::getCorners() const
{
    std::array<glm::vec3, 8> points;  
    glm::vec3 p(min.x, min.y, min.z);
    points[0] = p;
    p = glm::vec3(min.x, min.y, max.z);
    points[1] = p;
    p = glm::vec3(min.x, max.y, min.z);
    points[2] = p;
    p = glm::vec3(min.x, max.y, max.z);
    points[3] = p;
    p = glm::vec3(max.x, min.y, min.z);
    points[4] = p;
    p = glm::vec3(max.x, min.y, max.z);
    points[5] = p;
    p = glm::vec3(max.x, max.y, min.z);
    points[6] = p;
    p = glm::vec3(max.x, max.y, max.z);
    points[7] = p;

    return points;
}

bool MeshAABB::isInside(const glm::vec3& point) const
{
    return max.x > point.x&& max.y > point.y&& max.z > point.z&& min.x < point.x&& min.y < point.y&& min.z < point.z;
}

bool MeshAABB::isOnOrForwardPlane(const Plane& plane) const 
{
    float r = size.x * std::abs(plane.normal.x) + size.y * std::abs(plane.normal.y) +
        size.z * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(center);
}

bool MeshAABB::isOnFrustum(Frustum& camFrustum) const
{
    //NOTE(darius) TEMPO while scale dont work
    //if(transform.getScale() != glm::vec3{1,1,1})
    //    return true;

    //glm::vec3 globalCenter{ transform.matrix * glm::vec4(center, 1.f) };
    //glm::vec3 scl = transform.getScale();

    //MeshAABB globalAABB(globalCenter, scl.x, scl.y, scl.z);

    return (isOnOrForwardPlane(camFrustum.leftFace) &&
        isOnOrForwardPlane(camFrustum.rightFace) &&
        isOnOrForwardPlane(camFrustum.topFace) &&
        isOnOrForwardPlane(camFrustum.bottomFace) &&
        isOnOrForwardPlane(camFrustum.nearFace) &&
        isOnOrForwardPlane(camFrustum.farFace));
}

