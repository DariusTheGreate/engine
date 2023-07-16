#pragma once

#include <ScriptApi.h>
#include <GameState.h>
#include <Object.h>
#include <Animation.h>
#include <KeyboardState.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>

struct Player 
{
	float speed = 0.05f;
	int currAnim = 0;//0 - idle; 1 - runR; 2 - runL
};

class DefaultScript : public ScriptRoutine {
public:
	void start(ScriptArgument& args) override {
		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}
        
        auto* obj = args.obj;
		auto* scene = args.scene;
		//obj->addPointLight();//wont affect lighting 
		//obj->frozeObject();

		//scene->AddEmpty(555);

		run = SpriteAnimation(1,6,100);
		init = SpriteAnimation(4,8,100);

		initMesh = &scene->get_object_at(0)->getModel()->meshes[0];
		runMesh = &scene->get_object_at(1)->getModel()->meshes[0];
		runLeftMesh = &scene->get_object_at(3)->getModel()->meshes[0];

		obj = scene->get_object_at(2);
		obj->unhide();

		obj->setSpriteAnimation(init);
		//runMesh = new FlatMesh();//DANGER(darius) cant do that either
		//runMesh = scene->createFlatMesh();//nor that
	}

	void update(ScriptArgument& args) override {
        auto* obj = args.obj;
		auto* scene = args.scene;
		if (!instance || !obj) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}

		//obj = scene->get_object_at(2);

		if (instance->ks.get_d()) 
		{
			//obj->getModel().value().meshes[0] = runMesh;

			if (p.currAnim != 1) {
				obj->getModel()->meshes[0] = *runMesh;
				obj->setSpriteAnimation(run);
				p.currAnim = 1;
			}
			obj->getTransform().position.x += 1 * p.speed;
		}
		else if (instance->ks.get_a()) 
		{
			//obj->getModel().value().meshes[0] = runMesh;
			//obj->setSpriteAnimation(run);

			if (p.currAnim != 2) {
				obj->getModel()->meshes[0] = *runLeftMesh;
				obj->setSpriteAnimation(run);
				p.currAnim = 2;
			}
			obj->getTransform().position.x -= 1 * p.speed;
			//obj->getTransform().rotateBy(180, {0,0,1});
		}
		else if (instance->ks.get_q()) 
		{
			
		}
		else 
		{
			if (p.currAnim != 0) {
				obj->getModel()->meshes[0] = *initMesh;
				obj->setSpriteAnimation(init);
				p.currAnim = 0;
			}
			//obj->getModel().value().meshes[0] = initMesh;
			//obj->setSpriteAnimation(init);
		}
	}

	void setInstance(GameState* p) override
	{
		instance = p;
	}

	GameState* instance = nullptr;

	Player p;
	SpriteAnimation run;
	SpriteAnimation init;
	SpriteAnimation tmp;
	 
	//DANGER(darius) NOTE(darius) you cant create opengl entities, cause u have no opengl initialized here, TODO(darius) make some factory inside engine
	//FlatMesh runMesh;
	//Mesh initMesh;

	Mesh* runMesh = nullptr;
	Mesh* runLeftMesh = nullptr;
	Mesh* initMesh = nullptr;
};

