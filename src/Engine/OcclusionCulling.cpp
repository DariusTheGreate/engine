#include <windows.h>
#include <Engine/OcclusionCulling.h>
#include <Engine/Object.h>

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

			for(auto& p : points)
			{
				//glm::vec4 pnew = glm::vec4(p.x, p.y, p.z, 0) * obj2->getTransform().matrix;
				//p = {pnew.x, pnew.y, pnew.z};
				print(p);
			}

			print("\n");

			bool occlude = true;

			for(auto& p : points)
			{
				bool does = (intersectAABB(tmpAabb, glm::normalize((p) - origin))[0]);
				print(does);

				if(!does)	
					occlude = false;
			}

			if(occlude)
				obj2->cull();
			else
				if(!obj2->isCulled())
					obj2->uncull();

			println("");
		}
	}
}

void OcclusionCuller::rasterizeOccluders(std::vector<Object*> objs)
{
	auto* buff = raster.getBuff();	

	for (size_t i = 0; i < raster.getW(); ++i) {
		for(size_t j = 0; j < raster.getH(); ++j){
			*raster.at(i, j) = 128;//(i + j) > 255 ? 6 : (i + j);
		}
	}
	return;

	for (const auto& o : objs) 
	{
		auto& objectMeshes = o->getModel()->meshes;

		for (auto& m : objectMeshes) 
		{
			auto triangles = m.getTriangles();
			for(auto& t : triangles)
				raster.drawTri(t);
		}

	}

	//raster.printBuff();	
}

glm::vec3 OcclusionCuller::origin = {0,0,0};
Rasterizer OcclusionCuller::raster = Rasterizer(512, 256);
