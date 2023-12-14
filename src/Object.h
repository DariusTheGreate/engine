#pragma once
#include <vector>
#include <optional>
#include <string_view>
#include <functional>
#include <queue>
#include <memory>
#include <fstream>

#include <glm/glm.hpp>

#include <Model.h>
#include <Colider.h>
#include <RigidBody.h>
#include <Transform.h>
#include <Script.h>
#include <Scene.h>
#include <CubeMesh.h>
#include <PointLight.h>
#include <ParticleSystem.h>
#include <Animation.h>

class Object
{
public:
	Object(std::string name_in);

	Object(const Object& copy_me);

	Object& operator =(const Object& obj) = delete;

	void setupScript(EmptyScriptRoutine* r);

	void startScript();

	void updateScript(); 

	void renderObject(); 

	void addRigidBody(float mass = 0.0);

	std::optional<RigidBody>& getRigidBody();

	void addForce(glm::vec3 force); 

	void updateRigidBody(); 

	void frozeObject();

	void unfrozeObject();

	Transform& getTransform();

	Transform& getParentTransform();

	void moveTransform(glm::vec3 v);

	void addScript(Scene* scn, EmptyScriptRoutine* routine);
	
	std::optional<Script>& getScript();
	
	void setChildObjects(std::vector<Object*>&& objects);

	std::vector<Object*>& getChildObjects(); 

	void addChild(Object* obj);

	void traverseChilds(std::function<void(Object*)> functor);

	void traverseObjects(std::function<void(Object*)> functor, bool include_root = false);
	
	std::string& getName(); 

	void hide();

	void unhide();

	bool isHidden();

	bool& objectHiddenState();

	void cull();

	void uncull();

	bool isCulled();

	void serialize(std::ostream& file);

	void serializeAsPrefab(std::ofstream& file);

	void deserialize(std::ofstream& file);

	void addPointLight(PointLight&& pl = {}, glm::vec3 pos = { 0,0,0 });

	void addCollider(glm::vec3 size = {0,0,0}, glm::vec3 shift = { 0,0,0 });

	std::optional<Colider>& getColider(); 

	void addModel();

	void addModel(Mesh&& m);

	void addModel(std::string_view path);

	std::optional<Model>& getModel();
	
	void addParticleSystem(ParticleSystem&& ps);

	void updateParticleSystem(float dt);

	std::optional<ParticleSystem>& getParticleSystem();

	std::optional<PointLight>& getPointLight();

	bool& shadowCasterRef();

    void setDefaultMaterial();

	void setMaterial(const Material& m);

	std::optional<Material>& getMaterial();

	void setID(int id);

	int getID();


	void setSkeletalAnimation(SkeletalAnimation* anim);

	std::optional<SkeletalAnimation>& getSkeletalAnimation();

	std::optional<Animator<SpriteAnimation>>& getSpriteAnimator();

	void initAnimator(Animator<SpriteAnimation>&& animator_in);

    void addSpriteAnimation(SpriteAnimation&& anim);

	void updateSpriteAnimator(float dt);

	void updateSkeletalAnimation(float dt);

private:
	//TODO(darius) make it Component system
	// Obvious solution is to make use of virtual functions and stuff. And just store vector<Cmponent>
	// and then just call component[i] -> virtualUpdateFunction(). But Virtual function dispatch for each scne update frame its to much work.
	// So the idea is to allow object only one component of each type. And if you want more - just add subobject with this component.
	// NOTE(darius) dont forget optional cant contain reference or heap object
	// use indexing?
	// make it vector pointer to all components of specific type, for each component type?

	//NOTE(darius) How bad this is?
	//suppose O(amount of objects) = 10000 
	//sizeof(Object) ~= 1616b => 10000 * 1616b = 16Mb. Thats very little of waste!

	std::optional<Model> model;
	std::optional<RigidBody> rbody;
	std::optional<Colider> colider;
	std::optional<Script> script;//NOTE(darius) there shouldnt be many scripts, its bad
	std::optional<PointLight> pointLight;
	std::optional<Material> material;
	std::optional<ParticleSystem> particles;
	std::optional<SkeletalAnimation> skeletAnim;

    std::optional<Animator<SpriteAnimation>> spriteAnimator;
    //std::optional<Animator<SkeletalAnimation>> animator;

	Transform tr;
	std::string name;

	//NOTE(darius) maybe get rid of it?
	std::vector<Object*> childObjects = {};
	Object* parent = nullptr;

	//TODO(darius) looks like cull is same as hide, or is it
	bool objectHidden = false;
	bool objectCulled = false;
	bool shadowCaster = false;

	int ID = 0;
};

