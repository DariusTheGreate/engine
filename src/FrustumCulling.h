#pragma once

#include <Camera.h>

#include <glm/glm.hpp>

struct Plane
{
	glm::vec3 normal = { 0.f, 1.f, 0.f };
    float distance = 0.f;           	

    Plane() = default;

    Plane(const glm::vec3& p1, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

    float getSignedDistanceToPlane(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

class Frustum
{
public:
	Frustum(Camera& cam, float aspect, float fovY, float zNear, float zFar)
	{
	    const float halfVSide = zFar * tanf(fovY * .5f);
	    const float halfHSide = halfVSide * aspect;
	    const glm::vec3 frontMultFar = zFar * cam.getCameraFront();

	    nearFace = { cam.getCameraPos() + zNear * cam.getCameraFront(), cam.getCameraFront()};
	    farFace = { cam.getCameraPos() + frontMultFar, -cam.getCameraFront()};
	    auto Right = glm::normalize(glm::cross(cam.getCameraFront(), cam.getCameraUp()));

	    rightFace = { cam.getCameraPos(),
	                            glm::cross(frontMultFar - Right * halfHSide, cam.getCameraUp()) };
	    leftFace = { cam.getCameraPos(),
	                            glm::cross(cam.getCameraUp(), frontMultFar + halfHSide) };//NOTE(darius) broken
	    topFace = { cam.getCameraPos(),
                            glm::cross(Right, frontMultFar - cam.getCameraUp() * halfVSide) };
	    bottomFace = { cam.getCameraPos(),
                            glm::cross(frontMultFar + cam.getCameraUp() * halfVSide, Right) };
	}

    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};
