#pragma once

#include <glm/glm.hpp>

class Camera;
struct MeshAABB;

struct Plane
{
	glm::vec3 normal = { 0.f, 1.f, 0.f };
    float distance = 0.f;           	

    Plane() = default;

	Plane(const glm::vec3& p1, const glm::vec3& norm);

	float getSignedDistanceToPlane(const glm::vec3& point) const;

	void printMe();
};

class Frustum
{
public:
	Frustum() = default;
	
	Frustum(Camera& cam, float aspect, float fovY, float zNear, float zFar);

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
	static bool cull(const MeshAABB& aabb);

	static void updateFrustum();

    static void setFrustumSize(float w_in, float h_in);

private:
    static Frustum camFrustum;
   	static float w;
   	static float h; 
};

