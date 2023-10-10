#pragma once
#include <vector>
#include <AABB.h>
#include <Printer.h>
#include <GameState.h>

#include <glm/glm.hpp>
#include <glm/common.hpp>

class Object;

class OcclusionCuller
{
public:
	static bool cull(std::vector<Object*> objs);

	static glm::vec2 intersectAABB(MeshAABB aabb, glm::vec3 rayDir) 
	{
		glm::vec2 res;	
		res.x = rayAABBIntersection(aabb, rayDir, res.y);

		return res;
    }

    static bool rayAABBIntersection(MeshAABB aabb, const glm::vec3& direction, float& distance)
	{
	    using glm::max;
	    using glm::min;

	    const glm::vec3& aabbMin = aabb.min;
        const glm::vec3& aabbMax = aabb.max;

        //println(aabbMin, " " , aabbMax);

	    glm::vec3 dirfrac = 1.0f / direction;

	    float t1 = (aabbMin.x - origin.x) * dirfrac.x;
	    float t2 = (aabbMax.x - origin.x) * dirfrac.x;
	    float t3 = (aabbMin.y - origin.y) * dirfrac.y;
	    float t4 = (aabbMax.y - origin.y) * dirfrac.y;
	    float t5 = (aabbMin.z - origin.z) * dirfrac.z;
	    float t6 = (aabbMax.z - origin.z) * dirfrac.z;

	    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	    if (tmax < 0)
	    {
	        distance = tmax;
	        return false;
	    }

	    if (tmin > tmax)
	    {
	        distance = tmax;
	        return false;
	    }

	    distance = tmin;

	    return true;
	}

    static float fknMax(float a, float b)
    {
    	return a > b ? a : b;
    }

    static float fknMin(float a, float b)
    {
    	return a < b ? a : b;
    }

    static void updateOrigin()
    {
    	origin = GameState::cam.getCameraPos();
    }

private:	
	static glm::vec3 origin;
};
