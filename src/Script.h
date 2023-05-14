#pragma once

#include <functional>

class Scene;
class Object;

class Script 
{
public:
	Script(Scene* scn, Object* pobj, std::function<void(void*, void*)>&& st, std::function<void(void*, void*)>&& upd) : scriptScene(scn), parentObj(pobj), start(std::move(st)), update(std::move(upd))
	{
	}

	void set_scripts(std::function<void(void*, void*)>&& st, std::function<void(void*, void*)>&& upd)
	{
		start = st;
		update = upd;
	}

	void startScript()
	{
		start(scriptScene, parentObj);
	}

	void updateScript()
	{
		update(scriptScene, parentObj);
	}

private:
	std::function<void(void*, void*)> start;
	std::function<void(void*, void*)> update;
	
	Scene* scriptScene = nullptr;
	Object* parentObj = nullptr;

};

