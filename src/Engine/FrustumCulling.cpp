#include <Engine/FrustumCulling.h>

#include <Core/Printer.h>
#include <Engine/AABB.h>
#include <Engine/GameState.h>


Frustum FrustumCuller::camFrustum;
float FrustumCuller::w = 1920;
float FrustumCuller::h = 1080;

Plane::Plane(const glm::vec3& p1, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

float Plane::getSignedDistanceToPlane(const glm::vec3& point) const
{
	return glm::dot(normal, point) - distance;
}

void Plane::printMe()
{
	print(normal.x, " ",  normal.y, " ", normal.z, " ", distance, "\n");
}


Frustum::Frustum(Camera& cam, float aspect, float fovY, float zNear, float zFar)
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

bool FrustumCuller::cull(const MeshAABB& aabb)
{
	return aabb.isOnFrustum(camFrustum);
}

void FrustumCuller::updateFrustum()
{
	camFrustum = Frustum(GameState::instance->cam, w / h, glm::radians(GameState::GameState::instance->cam.getFov()), 0.01f, 100.0f); 	
}

void FrustumCuller::setFrustumSize(float w_in, float h_in)
{
	w = w_in;
	h = h_in;
}

