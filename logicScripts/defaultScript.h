#pragma once

#include <ScriptApi.h>
#include <GameState.h>
#include <Object.h>
#include <KeyboardState.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>

struct Player 
{
	float speed = 0.5;
};

class DefaultScript : public ScriptRoutine {
public:
	void start(ScriptArgument& args) override {
		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}
        
        //auto* obj = args.obj;
		//obj->addPointLight();//wont affect lighting 
		//obj->frozeObject();

		//instance->msg("Iam super duper mega start");// + obj->get_name());
	}

	void update(ScriptArgument& args) override {
		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}

        auto* obj = args.obj;

		instance->msg(std::to_string((int)(instance->connect)));
		if (instance->ks.get_d()) 
		{
			obj->getTransform().position.x += 1 * p.speed;
			//instance->msg("Iam super duper mega update");
		}

		if (instance->ks.get_a()) 
		{
			obj->getTransform().position.x -= 1 * p.speed;
			//instance->msg("Iam super duper mega update");
		}
	}

	void setInstance(GameState* p) override
	{
		instance = p;
	}

	GameState* instance = nullptr;
	Player p;
};

