#include <Script.h>

Script::Script(Scene* scn, Object* pobj, EmptyScriptRoutine* r) : scriptScene(scn), parentObj(pobj), routine(r)
{
}

Script::Script(Scene* scn, Object* pobj) : scriptScene(scn), parentObj(pobj)
{
}

void Script::set_scripts(EmptyScriptRoutine* r)
{
	routine = r;
}

void Script::set_scripts_from_file()
{
}

void Script::startScript()
{
	if (!routine)
		return;
	ScriptArgument tmpArgument = { scriptScene, parentObj, this };
	routine->start(tmpArgument);
}

void Script::updateScript()
{
	if (!routine)
		return;
	ScriptArgument tmpArgument = { scriptScene, parentObj, this };
	routine->update(tmpArgument);
}

void Script::setParentObject(Object* newParent)
{
	parentObj = newParent;
}

void Script::addVectorProperty(glm::vec3* property, std::string&& name)
{
	vector_properties.push_back(ScriptProperty<glm::vec3>(property, std::move(name)));
}

std::vector<ScriptProperty<glm::vec3>>& Script::getVectorProperties()
{
	return vector_properties;
}

void Script::addFloatProperty(float* property, std::string&& name)
{
	float_properties.push_back(ScriptProperty<float>(property, std::move(name)));
}

std::vector<ScriptProperty<float>>& Script::getFloatProperties()
{
	return float_properties;
}

