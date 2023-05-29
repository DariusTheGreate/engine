#pragma once

#include <functional>
#include <vector>
#include <memory>

class Scene;
class Object;
class Script;

template<typename T>
struct ScriptProperty
{
	ScriptProperty(glm::vec3* valp, std::string&& name_in) : val(valp), name(std::move(name_in)) {}

	T* operator ->()
	{
		return val;	
	}

	T* val;
	std::string name;
};

struct ScriptArgument
{
	Scene* scene;
	Object* obj;
	Script* script; 
};

class Script 
{
public:
	Script(Scene* scn, Object* pobj, std::function<void(ScriptArgument*)>&& st, std::function<void(ScriptArgument*)>&& upd) : scriptScene(scn), parentObj(pobj), start(std::move(st)), update(std::move(upd)) 
	{
	}

	void set_scripts(std::function<void(ScriptArgument*)>&& st, std::function<void(ScriptArgument*)>&& upd)
	{
		start = st;
		update = upd;
	}

	void startScript()
	{
		ScriptArgument tmpArgument = {scriptScene, parentObj, this}; 
		start(&tmpArgument);
	}

	void updateScript()
	{
		ScriptArgument tmpArgument = {scriptScene, parentObj, this}; 
		update(&tmpArgument);
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

private:
	std::function<void(ScriptArgument*)> start;
	std::function<void(ScriptArgument*)> update;
	
	Scene* scriptScene = nullptr;
	Object* parentObj = nullptr;

	std::vector<ScriptProperty<glm::vec3>> vector_properties;
};
