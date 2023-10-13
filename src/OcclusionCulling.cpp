#include <OcclusionCulling.h>
#include <Object.h>

#include <algorithm>

void OcclusionCuller::cull(std::vector<Object*> objs)
{
	//println("dir = ",  -glm::normalize(aabb.center - origin), " ", intersectAABB(aabb, glm::normalize(aabb.center - origin)));	
	//intersectAABB(aabb, glm::normalize(aabb.center - origin));

	for(auto& obj : objs)
	{
		obj->uncull();
	}

	std::vector<Object*> sortedObjs = objs;

	auto o = origin;
	std::sort(objs.begin(), objs.end(), [o](Object* a, Object* b){ return glm::distance(o, a->getTransform().getPosition()) > glm::distance(o, b->getTransform().getPosition()); });

	for(auto& obj : sortedObjs)
	{
		if(!obj->getModel())
			continue;

		for(auto& obj2 : sortedObjs)
		{
			if(!obj2->getModel() || obj2 == obj)
				continue;
		
			if(obj->getModel()->meshes.size() == 0 || obj2->getModel()->meshes.size() == 0)
				continue;

			auto tmpAabb = obj->getModel()->meshes[0].getAABB();
			auto tmpAabb2 = obj2->getModel()->meshes[0].getAABB();

			auto points = tmpAabb2.getCorners();

			bool occlude = true;

			for(auto& p : points)
			{
				bool does = (intersectAABB(tmpAabb, glm::normalize(p - origin))[0]);
				//print(does);

				if(!does)	
					occlude = false;
			}

			if(occlude)
				obj2->cull();
			else
				if(!obj2->is_culled())
					obj2->uncull();

			//println("");
		}
	}
}

glm::vec3 OcclusionCuller::origin = {0,0,0};
