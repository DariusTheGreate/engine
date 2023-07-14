#include <ScriptApi.h>

//TODO(darius) throws exception
EmptyScriptRoutine::EmptyScriptRoutine(std::string_view path, GameState* inst) : instance(inst)
{
	 //"E:/own/programming/engine/logicScripts/EngineLogic/x64/Debug"
	 boost::dll::fs::path lib_path(path);

	 plugin = boost::dll::import_symbol<ScriptRoutine>(
		 lib_path / "EngineLogic",
		 "script",
		 boost::dll::load_mode::append_decorations
	 );

	 if(instance)
		 plugin->setInstance(instance);
}

void EmptyScriptRoutine::update(ScriptArgument& arg) {
	//plugin->update(arg);
}

void EmptyScriptRoutine::start(ScriptArgument& arg) {
	//plugin->start(arg);
}
