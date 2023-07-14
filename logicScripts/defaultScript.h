#pragma once

#include <ScriptApi.h>
#include <GameState.h>
#include <Object.h>
#include <KeyboardState.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>

class DefaultScript : public ScriptRoutine {
public:
	void start(ScriptArgument& args) override {
		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}
        
        auto* obj = args.obj;
		obj->addPointLight();//wont affect lighting 
		obj->frozeObject();

		instance->msg("Iam super duper mega start");// + obj->get_name());
	}

	void update(ScriptArgument& args) override {
		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}

        auto* obj = args.obj;

		instance->msg(std::to_string((int)(instance->connect)));
		if (instance->ks.get_w()) 
		{
			obj->getTransform().position.x += 1;
			instance->msg("Iam super duper mega update");
		}
	}

	void setInstance(GameState* p) override
	{
		instance = p;
	}

	GameState* instance = nullptr;
};

