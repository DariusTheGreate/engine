#ifdef _WIN32
#include <windows.h>
#endif
#include <Engine/OcclusionCulling.h>
#include <Engine/Object.h>
#include <Engine/Editor.h>

#include <Engine/UI.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <limits>
#include <unordered_set>
#include <chrono>

void OcclusionCuller::cull(std::vector<Object*> objs)
{
	culledCount = 0;
	consideredCount = 0;
	skippedDraws = 0;
	occluderCount = 0;

	using clock = std::chrono::steady_clock;
	auto tStart = clock::now();

	const int W = static_cast<int>(raster.getW());
	const int H = static_cast<int>(raster.getH());
	const float Wf = static_cast<float>(W);
	const float Hf = static_cast<float>(H);

	raster.clearDepth();

	const int sw = Editor::window ? Editor::window->getWidth() : W;
	const int sh = Editor::window ? Editor::window->getHeight() : H;
	const glm::mat4 view = GameState::instance->cam.getBasicLook();
	const glm::mat4 proj = GameState::instance->cam.getPerspective(sw, sh);
	const glm::mat4 VP = proj * view;

	auto project = [&](const glm::mat4& M, const glm::vec3& p, glm::vec2& s, float& d) -> bool
	{
		glm::vec4 clip = M * glm::vec4(p, 1.0f);
		if(clip.w <= 1e-4f)
			return false;
		float invw = 1.0f / clip.w;
		s.x = (clip.x * invw * 0.5f + 0.5f) * Wf;
		s.y = (clip.y * invw * 0.5f + 0.5f) * Hf;
		d = clip.w;
		return true;
	};

	struct Item
	{
		Mesh* mesh;
		glm::mat4 mvp;
		int x0, y0, x1, y1;
		float dNear;       
		float area;        
		bool testable;     
	};

	std::vector<Item> items;
	std::unordered_set<Mesh*> seen;

	auto collect = [&](Object* obj)
	{
		obj->uncull();

		if(obj->isHidden() || !obj->getModel())
			return;

		glm::mat4 model = glm::scale(obj->getTransform().matrix, glm::vec3(0.1f, 0.1f, 0.1f));
		glm::mat4 mvp = VP * model;

		for(auto& m : obj->getModel()->meshes)
		{
			if(!seen.insert(&m).second) 
				continue;

			m.setOccluded(false);
			m.updateAABB(obj->getTransform());
			MeshAABB a = m.getAABB();

			MeshAABB box;
			box.min = glm::min(a.min, a.max);
			box.max = glm::max(a.min, a.max);

			Item it;
			it.mesh = &m;
			it.mvp = mvp;
			it.testable = true;

			float minX = 1e30f, minY = 1e30f, maxX = -1e30f, maxY = -1e30f, dNear = 1e30f;
			for(const auto& c : box.getCorners())
			{
				glm::vec2 s;
				float d;
				if(!project(VP, c, s, d)) 
				{
					it.testable = false;
					break;
				}
				minX = glm::min(minX, s.x); maxX = glm::max(maxX, s.x);
				minY = glm::min(minY, s.y); maxY = glm::max(maxY, s.y);
				dNear = glm::min(dNear, d);
			}

			if(it.testable && (maxX < 0 || minX > Wf - 1 || maxY < 0 || minY > Hf - 1))
				it.testable = false;

			if(it.testable)
			{
				it.x0 = std::max(0, static_cast<int>(std::floor(minX)));
				it.y0 = std::max(0, static_cast<int>(std::floor(minY)));
				it.x1 = std::min(W - 1, static_cast<int>(std::ceil(maxX)));
				it.y1 = std::min(H - 1, static_cast<int>(std::ceil(maxY)));
				it.dNear = dNear;
				it.area = (maxX - minX) * (maxY - minY);
			}

			items.push_back(it);
		}
	};

	for(auto& obj : objs)
	{
		if(!obj)
			continue;
		collect(obj);
		obj->traverseObjects([&collect](Object* child){ collect(child); }, false);
	}

	consideredCount = static_cast<int>(items.size());
	auto tCollect = clock::now();

	std::vector<int> order;
	order.reserve(items.size());
	for(int i = 0; i < static_cast<int>(items.size()); ++i)
		if(items[i].testable)
			order.push_back(i);

	std::sort(order.begin(), order.end(), [&](int a, int b){ return items[a].area > items[b].area; });

	auto rasterTri = [&](const glm::mat4& mvp, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		glm::vec2 s0, s1, s2;
		float d0, d1, d2;
		if(project(mvp, p0, s0, d0) && project(mvp, p1, s1, d1) && project(mvp, p2, s2, d2))
			raster.rasterizeTriangleDepth(s0, s1, s2, d0, d1, d2);
	};

	int budget = std::min(static_cast<int>(order.size()), occluderBudget);
	int trisDone = 0;
	for(int k = 0; k < budget && trisDone < occluderTriBudget; ++k)
	{
		Item& it = items[order[k]];
		auto& verts = it.mesh->getVerticesRef();
		auto& inds = it.mesh->getIndicesRef();

		if(!inds.empty())
		{
			for(size_t t = 0; t + 2 < inds.size() && trisDone < occluderTriBudget; t += 3, ++trisDone)
				rasterTri(it.mvp, verts[inds[t]].Position, verts[inds[t + 1]].Position, verts[inds[t + 2]].Position);
		}
		else
		{
			for(size_t t = 0; t + 2 < verts.size() && trisDone < occluderTriBudget; t += 3, ++trisDone)
				rasterTri(it.mvp, verts[t].Position, verts[t + 1].Position, verts[t + 2].Position);
		}
		++occluderCount;
	}
	auto tRaster = clock::now();

	for(Item& it : items)
	{
		if(!it.testable)
			continue;

		const float eps = it.dNear * 0.01f + 1e-4f; 
		bool occluded = true;
		for(int y = it.y0; y <= it.y1 && occluded; ++y)
			for(int x = it.x0; x <= it.x1; ++x)
				if(raster.depthAt(x, y) >= it.dNear - eps)
				{
					occluded = false;
					break;
				}

		if(occluded)
		{
			it.mesh->setOccluded(true);
			++culledCount;
		}
	}

	auto tTest = clock::now();

	if(debugLog || GameState::occlusionCullEnabled)
		raster.depthToBuff(); 

	if(debugLog)
	{
		auto ms = [](auto a, auto b){ return std::chrono::duration<double, std::milli>(b - a).count(); };
		println("[occlusion] meshes=", consideredCount, " occluders=", occluderCount,
			" culled=", culledCount,
			" | collect=", ms(tStart, tCollect), "ms raster=", ms(tCollect, tRaster),
			"ms test=", ms(tRaster, tTest), "ms total=", ms(tStart, tTest), "ms");
	}
}

void OcclusionCuller::rasterizeOccluders(std::vector<Object*> objs)
{
	auto* buff = raster.getBuff();

	if (buff) {
		UI::pushCustomWindow({
			"rasterizedImage",
			[]() {
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
Rasterizer OcclusionCuller::raster = Rasterizer(256, 128);
int OcclusionCuller::culledCount = 0;
int OcclusionCuller::consideredCount = 0;
bool OcclusionCuller::debugLog = false;
int OcclusionCuller::skippedDraws = 0;
int OcclusionCuller::occluderCount = 0;
int OcclusionCuller::occluderBudget = 48;
int OcclusionCuller::occluderTriBudget = 5000;

