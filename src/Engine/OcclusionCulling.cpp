#include <windows.h>
#include <Engine/OcclusionCulling.h>
#include <Engine/Object.h>

#include <Engine/UI.h>

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

	for (uint32_t i = 0; i < raster.getW(); ++i) {
		for(uint32_t j = 0; j < raster.getH(); ++j){
			*raster.at(i, j) = (i + j) > 255 ? 200 : (i + j);
		}
	}

	/*for (const auto& o : objs)
	{
		auto& objectMeshes = o->getModel()->meshes;

		for (auto& m : objectMeshes) 
		{
			auto triangles = m.getTriangles();
			for(auto& t : triangles)
				raster.drawTri(t);
		}

	}*/

	if (buff) {
		UI::pushCustomWindow({
			"rasterizedImage",
			[]() {
				//std::vector<uint8_t> buff(255, 128*128);
				//static Texture t(GameState::engine_path + "buff.jpg", GL_RGB, GL_RGB);
				//t = Texture(buff.data(), 128, 128);
				Texture t = raster.genTexture();

				ImGui::Image((void*)(intptr_t)(t.get_texture()), {static_cast<float>(raster.getW()), static_cast<float>(raster.getH())});
				if (ImGui::Button("Save")) 
				{
					raster.saveBuffToImage();
				}
			}
		});
	}

	//raster.printBuff();	
}

glm::vec3 OcclusionCuller::origin = {0,0,0};
Rasterizer OcclusionCuller::raster = Rasterizer(512, 256);

