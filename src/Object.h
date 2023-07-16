#pragma once
#include <vector>
#include <optional>
#include <string_view>
#include <functional>
#include <queue>
#include <memory>

#include <glm/glm.hpp>

#include <Model.h>
#include <Colider.h>
#include <RigidBody.h>
#include <Transform.h>
#include <Script.h>
#include <Scene.h>
#include <CubeMesh.h>
#include <PointLight.h>
#include <LightingShaderRoutine.h>
#include <ParticleSystem.h>
#include <Animator.h>

class Object
{
public:
	Object(std::string name_in);

	Object(std::string name_in, Shader model_shader, LightingShaderRoutine& shaderRoutine_in);

	Object(std::string name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, std::string_view model_path_in, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
																									Scene* scn, EmptyScriptRoutine* routine,
																									bool gammaShader = false, bool rotateTextures = false);

	Object(Object* parentObject, Mesh& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in);

	Object(std::string&& name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, Mesh& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
																					Scene* scn, EmptyScriptRoutine* routine, bool active = true);

	Object(std::string&& name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, Model& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
																					Scene* scn, EmptyScriptRoutine* routine, bool active = true);

	void setupScript(EmptyScriptRoutine* r);

	void startScript();

	void updateScript(); 

	void updateAnimator(float dt);

	void renderObject(); 

	void updateParticleSystem(float dt);

	void apply_force(glm::vec3 force); 

	void updatePos(); 

	glm::vec3 get_pos(); 
	
	//TODO(darius) fuck you, incapsulation
	glm::vec3& get_pos_ref();

	std::optional<Colider>& getColider(); 

	std::optional<RigidBody>& getRigidBody();

	std::optional<Model>& getModel();

	void frozeObject();

	void unfrozeObject();

	Transform& getTransform();

	Transform& getParentTransform();
	
	void addScript(Scene* scn, EmptyScriptRoutine* routine);
	std::optional<Script>& getScript();
	
	void set_child_objects(std::vector<Object*>&& objects);

	std::vector<Object*>& get_child_objects(); 

	void addChild(Object* obj);

	void traverseChilds(std::function<void(Object*)> functor);

	void traverseObjects(std::function<void(Object*)> functor, bool include_root = false);
	
	std::string& get_name(); 

	void hide();

	void unhide();

	bool& object_hidden_state();

	void addPointLight(PointLight&& pl = {}, glm::vec3 pos = { 0,0,0 });

	void addCollider();

	void addModel(Mesh&& m, Shader sv, LightingShaderRoutine routine);

	void addModel(Shader sv, LightingShaderRoutine routine);

	//TODO(darius) when loading big models add threading
	void addModel(std::string_view path, Shader sv, LightingShaderRoutine routine, bool rotate = false);

	void addModel(std::string_view path);
	
	void addParticleSystem(ParticleSystem&& ps);

	std::optional<ParticleSystem>& getParticleSystem();

	std::optional<PointLight>& getPointLight();

    void setDefaultMaterial();

	void setMaterial(const Material& m);

	std::optional<Material>& getMaterial();

	void setAnimator(Animation* anim);

	std::optional<Animator>& getAnimator();

    void addSpriteAnimation(SpriteAnimation&& anim);
	SpriteAnimation excnahgeSpriteAnimation(SpriteAnimation&& anim);
	void setSpriteAnimation(SpriteAnimation& anim);
    std::optional<SpriteAnimation>& getSpriteAnimation();
	void updateSpriteAnimation(float dt);

private:
	//TODO(darius) make it Component system
	// Obvious solution is to make use of virtual functions and stuff. And just store vector<Cmponent>
	// and then just call component[i] -> virtualUpdateFunction(). But Virtual function dispatch for each scne update frame its to much work.
	// So the idea is to allow object only one component of each type. And if you want more - just add subobject with this component.
	// NOTE(darius) dont forget optional cant contain reference or heap object
	// use indexing?
	// make it vector pointer to all components of specific type, for each component type?
	std::optional<Model> model;
	std::optional<RigidBody> rbody;
	std::optional<Colider> colider;
	std::optional<Script> script;
	std::optional<PointLight> pointLight;
	std::optional<Material> material = std::nullopt;
	std::optional<ParticleSystem> particles;
	std::optional<Animator> animator;
    std::optional<SpriteAnimation> spriteAnimation;

	Transform tr;
	std::string name;

	std::vector<Object*> child_opbjects = {};
	Object* parent = nullptr;

	bool object_hidden = false;
};

