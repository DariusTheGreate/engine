#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include <ScriptApi.h>

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
	Script(Scene* scn, Object* pobj, EmptyScriptRoutine* r) : scriptScene(scn), parentObj(pobj), routine(r)
	{
	}

    Script(Scene* scn, Object* pobj) : scriptScene(scn), parentObj(pobj)
	{
	}

	void set_scripts(EmptyScriptRoutine* r)
	{
        routine = r;
	}

    void set_scripts_from_file()
    {
        
    }

	void startScript()
	{
        if(!routine)
            return;
		ScriptArgument tmpArgument = {scriptScene, parentObj, this}; 
		routine->start(tmpArgument);
	}

	void updateScript()
	{
        if(!routine)
            return;
		ScriptArgument tmpArgument = {scriptScene, parentObj, this}; 
		routine->update(tmpArgument);
	}

	void setParentObject(Object* newParent)
	{
		parentObj = newParent;
	}

	void addVectorProperty(glm::vec3* property, std::string&& name)
	{
		vector_properties.push_back(ScriptProperty<glm::vec3>(property, std::move(name)));
	}

	std::vector<ScriptProperty<glm::vec3>>& getVectorProperties()
	{
		return vector_properties;		
	}

	void addFloatProperty(float* property, std::string&& name)
	{
		float_properties.push_back(ScriptProperty<float>(property, std::move(name)));
	}

	std::vector<ScriptProperty<float>>& getFloatProperties()
	{
		return float_properties;		
	}

private:
	//std::function<void(ScriptArgument*)> start;
	//std::function<void(ScriptArgument*)> update;
	EmptyScriptRoutine* routine;
	
	Scene* scriptScene = nullptr;
	Object* parentObj = nullptr;

	std::vector<ScriptProperty<glm::vec3>> vector_properties;
	std::vector<ScriptProperty<float>> float_properties;
};
