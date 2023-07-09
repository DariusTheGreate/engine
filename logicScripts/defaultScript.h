#pragma once

#include <ScriptApi.h>
#include <GameState.h>
//#include <Object.h>

#include <boost/config.hpp> 
#include <iostream>

class DefaultScript : public ScriptRoutine {
public:
	void start(ScriptArgument& args) override {
		if(!instance)
			std::cout << "Iam super duper mega start\n";
        
        //auto* obj = args.obj;

		instance->msg("Iam super duper mega start");// + obj->get_name());
	}
	void update(ScriptArgument& args) override {
		//instance->msg("Iam super duper mega update");
	}

	void setInstance(GameState* p) override
	{
		instance = p;
	}

	GameState* instance = nullptr;
};
