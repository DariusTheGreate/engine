#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <Engine/ScriptApi.h>

class Scene;
class Object;
class Script;

template<typename T>
struct ScriptProperty
{
	ScriptProperty(T* valp, std::string&& name_in) : val(valp), name(std::move(name_in)) {}

	T* operator ->()
	{
		return val;	
	}

	T* get_val()
	{
		return val;
	}

	T* val;
	std::string name;
};

class Script 
{
public:
	Script(Scene* scn, Object* pobj, DllScriptRoutine* r);

	Script(Scene* scn, Object* pobj);

	void set_scripts(DllScriptRoutine* r);

	void set_scripts_from_file();

	void startScript();

	void updateScript();

	void setParentObject(Object* newParent);

	void addVectorProperty(glm::vec3* property, std::string&& name);

	std::vector<ScriptProperty<glm::vec3>>& getVectorProperties();

	void addFloatProperty(float* property, std::string&& name);

	std::vector<ScriptProperty<float>>& getFloatProperties();

	DllScriptRoutine* getRoutine();

private:
	//std::function<void(ScriptArgument*)> start;
	//std::function<void(ScriptArgument*)> update;
	DllScriptRoutine* routine = nullptr;
	
	Scene* scriptScene = nullptr;
	Object* parentObj = nullptr;

	std::vector<ScriptProperty<glm::vec3>> vector_properties;
	std::vector<ScriptProperty<float>> float_properties;
};
