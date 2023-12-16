#pragma once
#include <vector>
#include <memory>
#include <bitset>
#include <mutex>

#include <Engine/Object.h>
#include <Engine/GameState.h>
#include <Physics/RigidBody.h>
#include <Physics/Colider.h>
#include <Engine/ParticleSystem.h>
#include <Engine/Script.h>
#include <Engine/Animation.h>
#include <Engine/Camera.h>
#include <Core/Timer.h> 
#include <Rendering/BatchingCache.h>
#include <Core/ThreadPool.h>
#include <Networking/NetworkSynchronizer.h>
#include <Engine/Terrain.h>

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
		return new Object(std::forward<Args>(args)...);

		//assert(filled_pools < mem_pools.size(), "mem_pool size < filled pools value");
		/*Pool& curp = mem_pools[filled_pools];
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
		*/
	}
	
	//TODO(darius) test this shit like never before
	void destroy(Object* p) 
	{
		delete p;
		return;

		/*size_t addrp = (size_t)(p);
		for (int i = 0; i < filled_pools; ++i) {
			if (addrp > (size_t)mem_pools[i].p && addrp < ((size_t)mem_pools[i].p + mem_pools[i].busy * sizeof(T))) {
				for (int j = 0; j < mem_pools[i].busy; ++j) {
					//update bitmask	
					mem_pools[i].set[j] = 0;
				}
			}
		}*/
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

	Object* createEntity(Object* po, std::string path, bool rotateTextures);

	Object* AddEmpty(int i);

	Object* AddObject(const std::string& name);

	Object* createSubobject(Object* obj, int i);

	void destroyObject(size_t id);

	void destroyObject(Object* obj);

	void destroyObject(std::string_view name);

	void updateScene();

	void updateSceneAABBs();

	void renderScene();

	void renderParticles();

	void updateAnimators(float dt);

	void updateSpriteAnimations(float dt);

	Object* get_object_at(int i);

	Object* getObjectByName(const std::string& name);

	Object* getObjectByID(int ID);

	std::vector<Object*>& getObjects();

	int getEmptyIndex();

	void start_scripts();

	void updateObjectsIDs();

	//TODO(darius) make it factory or something
	//DANGER(darius) currently theres no garbage collection on this. So leaks are possible.
	FlatMesh* createFlatMesh();

	template<typename... Args>
	Mesh* createMesh(Args&&... args)
	{
		return new Mesh(std::forward<Args>(args)...);	
	}

	void deleteFlatMesh(FlatMesh* mesh);

	EmptyScriptRoutine* createRoutine(std::string path);

	void deleteRoutine(EmptyScriptRoutine*);

	Camera* createCamera();

	void deleteCamera(Camera* cam);

	void addCameraToScene(Camera* cam);

	std::vector<Camera*>& getSceneCameras();

	Camera* getCameraAt(int id);

	void batchProbeSimilarObjects();

	void recoverBatchedObjects();

	void updateObjectsLODs(); 

	bool recoverBatch(Object* origin);

	BatchingCache& getBatchCache();

	void addObjectToNetSync(Object* obj);

	NetworkSynchronizer& getNetworkSynchronizer();

	void addTerrain(Terrain&& trin);

public:
	//NOTE(darius) I know it can be done much better, but i dont want to waste time on it NOW, will have to return later to it anyway
	void serialize(std::string_view path);

	void serializePrefab(Object* obj, std::string_view path);

	void deserialize(std::string_view path);

	void parseSynchronizationMsg(std::string msg);

	void parseScene(std::string_view scene);

	std::string readFileToString(std::string_view path);

	std::string extractNameFromToken(std::string_view);

	bool extractHiddenStateFromToken(std::string_view tkn);

	Transform extractTransformFromToken(std::string_view);

	Model extractModelFromToken(std::string_view);

	Model extractMeshesFromToken(std::string_view);

	std::optional<Colider> extractColliderFromToken(std::string_view);

	std::optional<RigidBody> extractRigidBodyFromToken(std::string_view);

	std::optional<SpriteAnimator> extractSpriteAnimatorFromToken(std::string_view);

	std::string extractScriptFromToken(std::string_view);

	std::optional<PointLight> extractPointLightFromToken(std::string_view);

	std::optional<ParticleSystem> extractParticleSystemFromToken(std::string_view);

	//TODO(darius) make it own module or somtng
	glm::vec3 extractVectorFromToken(std::string_view);

	glm::vec4 extractVector4FromToken(std::string_view);

	bool extractBoolFromToken(std::string_view);

	int extractCurrAnimIdFromToken(std::string_view tkn, int start = 0);

private:

	void init_memory();
	
	void update_objects();

	void update_scripts();

private:
	std::vector<Object*> sceneObjects;//more common way is to store indexes
	std::vector<Terrain> terrains;
	std::mutex sceneLock;
	//std::vector<Object*> currentlyBatchedObjects;
	BatchingCache batcher;
	//TODO(darius) its heavy to sync all of the object data. So although by default thats waht we do now - 
	//			   - its important to allow to create networkSynchronizer<Transfrom>, NetSync<string>, etc. and store vetor of them
	NetworkSynchronizer networkSync;

	//ThreadPool workersUsed;//NOTE(darius) just messing around

	//TODO(darius) make it use memory manager
	std::vector<Camera*> sceneCameras;

	int idCamera = -1;//-1 means default camera;

	SceneMemoryManager<> mem_man;
	int emptyIndex = 0;

	Timer CheckColTime;
};

