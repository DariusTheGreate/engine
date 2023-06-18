#pragma once
#include <vector>
#include <memory>
#include <bitset>

#include <Object.h>
#include <GameState.h>
#include <LightingShaderRoutine.h>

//TODO(darius) make it packed in 64bytes cache line
constexpr size_t CHUNK_COUNT = 10;
constexpr size_t CHUNK_SZ = sizeof(Object) * CHUNK_COUNT;
constexpr size_t POOL_SZ = 3;

//kinda cool, but kinda cringe..The Idea is to make object destory fast, and collect garbage after
//TODO(darius) test this shit 
//TODO(darius) objects replication(if there is object copy - just point to same location)
template<typename T = Object, size_t CHUNK_COUNT = CHUNK_COUNT, size_t CHUNK_SZ = CHUNK_SZ, size_t POOL_SZ = POOL_SZ>
class SceneMemoryManager
{
public:
	struct Pool 
	{
		char* p;
		size_t busy;
		size_t empty;
		std::bitset<CHUNK_COUNT> set;
	};

public:
	void allocate(size_t obj_count) 
	{
		if (obj_count == 0)
			return;

		if (obj_count <= POOL_SZ)
			create_new_pool();


		while(obj_count > POOL_SZ) {
			create_new_pool();
			obj_count -= POOL_SZ;
		}
	}

	void eliminate_fragmentation(void* p) 
	{
		//check bitmasks here
	}
	
	template<typename... Args>
	Object* construct(Args&&... args) 
	{
		assert(filled_pools < mem_pools.size(), "mem_pool size < filled pools value");
		Pool& curp = mem_pools[filled_pools];
		//std::construct_at c++20
		char* construction_ptr = curp.p + curp.busy * sizeof(T);
		::new (construction_ptr) Object(std::forward<Args>(args)...);
		//auto* pp = (Object*)curp.p;
		//pp->get_name();
		curp.set[curp.busy++] = 1;
		curp.empty--;
		if (curp.empty == 0)
			filled_pools++;
		return (T*)construction_ptr;
	}
	
	//TODO(darius) test this shit like never before
	void destroy(void* p) 
	{
		size_t addrp = (size_t)(p);
		for (int i = 0; i < filled_pools; ++i) {
			if (addrp > (size_t)mem_pools[i].p && addrp < ((size_t)mem_pools[i].p + mem_pools[i].busy * sizeof(T))) {
				for (int j = 0; j < mem_pools[i].busy; ++j) {
					//update bitmask	
					mem_pools[i].set[j] = 0;
				}
			}
		}
	}

private:
	std::allocator<char> bytes;

	std::vector<Pool> mem_pools;
	size_t filled_pools = 0;;

	void create_new_pool() 
	{
		char* p = bytes.allocate(POOL_SZ * CHUNK_SZ);
		
		//TODO(darius) check p here

		Pool newp = { p, 0, POOL_SZ };
		mem_pools.push_back(newp);
	}
};

class Scene 
{
public:
	Scene()
	{
		init_memory();
		start_scripts();
	}

	template<typename... Args>
	Object* createObject(Args&&... args) {
		Object* pt = mem_man.construct(std::forward<Args>(args)...);
		sceneObjects.push_back(pt);
		pt->startScript();

		return pt;
	}

	Object* createEntity(Object* po, std::string path, Shader sv, LightingShaderRoutine shaderRoutine_in, bool rotateTextures = false) {
		Model m = Model(path, shaderRoutine_in, sv, rotateTextures);
		auto meshes = m.loadModel();

		std::vector<Object*> subobjects;
		subobjects.reserve(meshes.size());
		for (int i = 0; i < meshes.size(); ++i) {
			Object* pt = mem_man.construct(po, meshes[i], sv, shaderRoutine_in);
			std::cout << "created pt\n";
			subobjects.push_back(pt);
		}
		
		po->set_child_objects(std::move(subobjects));

		po->startScript();
		return po;
	}

	Object* AddEmpty(int i)
	{
		Object* pt;
		pt = mem_man.construct("empty " + std::to_string(i));	
		sceneObjects.push_back(pt);
		return pt;
	}

	Object* createSubobject(Object* obj, int i)
	{
		Object* pt;
		pt = mem_man.construct("empty " + std::to_string(i));	
		obj->addChild(pt);

		return pt;
	}

	void destroyObject(size_t id)
	{
		auto* ptr = sceneObjects[id];
		mem_man.destroy(ptr);
		sceneObjects[id] = nullptr;
	}

	void updateScene() {
		update_objects();
	}

	void renderScene()
	{
		for (int i = 0; i < sceneObjects.size(); ++i) {
			if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
				continue;	
			sceneObjects[i]->renderObject();
		}
	}

	void renderParticles()
	{
		for(int i = 0; i < sceneObjects.size(); ++i){
			if(!sceneObjects[i]->getParticleSystem())
				continue;

	        sceneObjects[i]->getParticleSystem()->updateUniform3DDistribution(glfwGetTime());
	        sceneObjects[i]->getParticleSystem()->renderParticles();
	    }
	}

	void updateAnimators(float dt)
	{
		for(auto& obj : sceneObjects){
			if(!obj->getAnimator())
				continue;
			obj->updateAnimator(dt);
		}
	}

	Object* get_object_at(int i) 
	{
		return sceneObjects.at(i);
	}

	std::vector<Object*>& get_objects()
	{
		return sceneObjects;
	}

private:

	void init_memory() 
	{
		mem_man.allocate(1000);
	}

	void start_scripts()
	{
		for(auto& i : sceneObjects)
		{
			i->startScript();
		}
	}
	
	void update_objects() {
		//now its traverse of objects and update. Its much better to do it in one traverse
		//TODO(darius) make it separated threads for collisions and rendering and update?

		for (int i = 0; i < sceneObjects.size(); ++i) {
			if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
				continue;


			sceneObjects[i]->updateScript();


			if(sceneObjects[i]->getColider() && !sceneObjects[i]->getColider()->is_active()){
				sceneObjects[i]->updatePos();
				continue;
			}

			//COLLISIONS RESOLUTION:
			// O(n^2) 
			// sort by tag + traverse in O(Nlg + N)?
			// make it two types of collision detection: 1) important collision inside renderer thread 2) queued colllision that processed in separate thread?

			bool is_there_collision = false;
			for (int j = 0; j < sceneObjects.size(); ++j) {
				if (
					i == j 
					|| !sceneObjects[i]->getColider()
					|| !sceneObjects[j]->getColider()
					|| sceneObjects[i]->getColider()->get_tag() != sceneObjects[j] -> getColider()->get_tag() 
					|| !sceneObjects[j]->getColider()->is_active()
				)	continue;

				auto collision_state = sceneObjects[i]->getColider()->gjk(&sceneObjects[i]->getColider().value(), &sceneObjects[j]->getColider().value());

				if (collision_state) {
					is_there_collision = collision_state;
					glm::vec3 epa = sceneObjects[i]->getColider()->get_epa();

					if(sceneObjects[i]->getRigidBody() && sceneObjects[j]->getRigidBody() && epa.x == epa.x && epa.y == epa.y && epa.z == epa.z){
						sceneObjects[i]->getRigidBody()->tr.position += glm::vec3{epa.x/4, epa.y/4, epa.z/4};
					}

					break;
				}
			}
			if (!is_there_collision) {
				sceneObjects[i]->updatePos();
			}
		}
	}

	void update_scripts()
	{
		for(auto& i : sceneObjects)
		{
			i->updateScript();
		}
	}

private:
	std::vector<Object*> sceneObjects;//more common way is to store indexes
	SceneMemoryManager<> mem_man;
};
