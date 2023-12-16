#pragma once

#include <Engine/Camera.h>
#include <Core/Printer.h>
#include <Engine/AABB.h>
#include <Engine/GameState.h>

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

	void printMe()
	{
		print(normal.x, " ",  normal.y, " ", normal.z, " ", distance, "\n");
	}
};

class Frustum
{
public:
	Frustum() = default;
	
	Frustum(Camera& cam, float aspect, float fovY, float zNear, float zFar)
	{
	    const float halfVSide = zFar * tanf(fovY * .5f);
	    const float halfHSide = halfVSide * aspect;

	    //print("\n", halfVSide, "\n", halfHSide, "\n");

	    glm::vec3 frontMultFar = zFar * cam.getCameraFront();

	    nearFace = { cam.getCameraPos() + zNear * cam.getCameraFront(), cam.getCameraFront()};
	    //nearFace.printMe();

	    farFace = { cam.getCameraPos() + frontMultFar, -cam.getCameraFront()};
	    //farFace.printMe();

	    rightFace = { cam.getCameraPos(),
	                            glm::cross(frontMultFar - cam.getCameraRight() * halfHSide, cam.getCameraUp()) };

	    leftFace = { cam.getCameraPos(),
	                            glm::cross(cam.getCameraUp(), frontMultFar + cam.getCameraRight() * halfHSide) };

	    topFace = { cam.getCameraPos(),
                            glm::cross(cam.getCameraRight(), frontMultFar - cam.getCameraUp() * halfVSide) };

	    bottomFace = { cam.getCameraPos(),
                            glm::cross(frontMultFar + cam.getCameraUp() * halfVSide, cam.getCameraRight()) };
	}

    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};

class FrustumCuller
{
public:
	static bool cull(MeshAABB aabb)
	{
        return aabb.isOnFrustum(camFrustum);
	}

	static void updateFrustum()
	{
		camFrustum = Frustum(GameState::instance->cam, w / h, glm::radians(GameState::GameState::instance->cam.getFov()), 0.01f, 100.0f); 	
	}

	static void setFrustumSize(float w_in, float h_in)
	{
		w = w_in;
		h = h_in;
	}

private:
    static Frustum camFrustum;//(GameState::cam, (float)1920 / (float)1080, glm::radians(GameState::cam.getFov()), 0.1f, 100.0f);
   	static float w;
   	static float h; 
};
