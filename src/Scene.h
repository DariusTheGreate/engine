#pragma once
#include <vector>
#include <memory>
#include <bitset>

#include <Object.h>
#include <GameState.h>
#include <LightingShaderRoutine.h>
#include <RigidBody.h>
#include <Colider.h>
#include <ParticleSystem.h>
#include <Script.h>
#include <Animation.h>

//TODO(darius) make it packed in 64bytes cache line
constexpr size_t CHUNK_COUNT = 10;
constexpr size_t POOL_SZ = 3;

//kinda cool, but kinda cringe..The Idea is to make object destory fast, and collect garbage after
//TODO(darius) test this shit 
//TODO(darius) objects replication(if there is object copy - just point to same location)
template<typename T = Object, size_t CHUNK_COUNT = CHUNK_COUNT, size_t POOL_SZ = POOL_SZ>
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
		//assert(filled_pools < mem_pools.size(), "mem_pool size < filled pools value");
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
		//TODO(darius) not 64!
		char* p = bytes.allocate(POOL_SZ * sizeof(Object) * CHUNK_COUNT);
		
		//TODO(darius) check p here
		Pool newp = { p, 0, POOL_SZ };
		mem_pools.push_back(newp);
	}
};

class Scene 
{
public:
	Scene();

	template<typename... Args>
	Object* createObject(Args&&... args) {
		Object* pt = mem_man.construct(std::forward<Args>(args)...);
		sceneObjects.push_back(pt);
		//pt->startScript();

		return pt;
	}

	Object* createEntity(Object* po, std::string path, Shader sv, LightingShaderRoutine shaderRoutine_in, bool rotateTextures);

	Object* AddEmpty(int i);
	Object* AddObject(const std::string& name);

	Object* createSubobject(Object* obj, int i);

	void destroyObject(size_t id);

	void updateScene();

	void renderScene();

	void renderParticles();

	void updateAnimators(float dt);
	void updateSpriteAnimations(float dt);

	Object* get_object_at(int i);

	std::vector<Object*>& get_objects();

	void start_scripts();

	//TODO(darius) make it factory or something
	FlatMesh* createFlatMesh();
	void deleteFlatMesh(FlatMesh* mesh);

	EmptyScriptRoutine* createRoutine(std::string path);
	void deleteRoutine(EmptyScriptRoutine*);

	//NOTE(darius) I know it can be done much better, but i dont want to waste time on it NOW, will return later to it anyway
	void serialize(std::string_view path);
	void deserialize(std::string_view path);

	std::string extractNameFromToken(std::string_view);
	bool extractHiddenStateFromToken(std::string_view tkn);
	Transform extractTransformFromToken(std::string_view);
	Model extractModelFromToken(std::string_view);
	Model extractMeshesFromToken(std::string_view);
	std::optional<Colider> extractColliderFromToken(std::string_view);
	std::optional<RigidBody> extractRigidBodyFromToken(std::string_view);
	std::optional<SpriteAnimation> extractSpriteAnimationFromToken(std::string_view);
	std::string extractScriptFromToken(std::string_view);

	glm::vec3 extractVectorFromToken(std::string_view);
	glm::vec4 extractVector4FromToken(std::string_view);
	bool extractBoolFromToken(std::string_view);

private:

	void init_memory();
	
	void update_objects();

	void update_scripts();

private:
	std::vector<Object*> sceneObjects;//more common way is to store indexes
	SceneMemoryManager<> mem_man;
};

