#pragma once

#include <boost/dll/import.hpp>
#include <string_view>

class Scene;
class Object;
class Script;
class GameState;

struct ScriptArgument
{
	Scene* scene;
	Object* obj;
	Script* script; 
};

class ScriptRoutine
{
public:
    virtual void start(ScriptArgument& arg) = 0;
    virtual void update(ScriptArgument& arg) = 0;
	virtual void setInstance(GameState* p) = 0;
};

class EmptyScriptRoutine 
{
public:
	EmptyScriptRoutine(std::string_view path, GameState* inst) : instance(inst)
	{
       /* / //"E:/own/programming/engine/logicScripts/EngineLogic/x64/Debug"
		boost::dll::fs::path lib_path(path);             

		plugin = boost::dll::import_symbol<ScriptRoutine>(        
			lib_path / "EngineLogic",                     
			"script",                                    
			boost::dll::load_mode::append_decorations          
		);

		if(instance)
			plugin->setInstance(instance);
		*/
	}

	EmptyScriptRoutine() = default;

    void update(ScriptArgument& arg){
        //plugin->update(arg);
    }

    void start(ScriptArgument& arg) {
        //plugin->start(arg);
    }

private:
	GameState* instance = nullptr;
    boost::shared_ptr<ScriptRoutine> plugin;
};

