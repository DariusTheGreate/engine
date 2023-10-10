#include <OcclusionCulling.h>
#include <Object.h>

bool OcclusionCuller::cull(std::vector<Object*> objs)
{
	//println("dir = ",  -glm::normalize(aabb.center - origin), " ", intersectAABB(aabb, glm::normalize(aabb.center - origin)));	
	//intersectAABB(aabb, glm::normalize(aabb.center - origin));

	for(auto& obj : objs)
	{
		if(!obj->getModel())
			continue;

		for(auto& obj2 : objs)
		{
			if(!obj2->getModel() || obj2 == obj)
				continue;
		
			auto tmpAabb = obj->getModel()->meshes[0].getAABB();
			auto tmpAabb2 = obj2->getModel()->meshes[0].getAABB();

			bool minInt = intersectAABB(tmpAabb, glm::normalize(tmpAabb2.min - origin))[0];
			bool maxInt = intersectAABB(tmpAabb, glm::normalize(tmpAabb2.max - origin))[0];

			if(minInt && maxInt)
				obj2->hide();
			else
				obj2->unhide();

			println(obj->get_name(), " ", obj2->get_name(), "min", minInt);
			println(obj->get_name(), " ", obj2->get_name(), "max", maxInt);
		}
	}

	return true;
}

glm::vec3 OcclusionCuller::origin = {0,0,0};
