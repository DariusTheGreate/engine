#pragma once

#include <Engine/ScriptApi.h>
#include <Engine/GameState.h>
#include <Engine/Object.h>
#include <Engine/Animation.h>
#include <Engine/KeyboardState.h>
#include <Core/Printer.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>
#include <chrono>

struct Player 
{
	float speed = 1.0f;
	bool inJump = false;
	int currAnim = 0;//0 - idle; 1 - runR; 2 - run; 3 - jump; 4 - attack
};

class DefaultScript : public ScriptRoutine {
public:
	//TODO(darius) pass script properts here
	void start(ScriptArgument& args) override {
		instance->debug_msg.append("Enter Script..\n");
		std::cout << "Enter Scrtipt..\n";

		if (!instance) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}

		//instance->debug_msg.append("Start Script..");
        
        auto* obj = args.obj;
		auto* scene = args.scene;

		//obj->addPointLight();//wont affect lighting 
		//obj->frozeObject();

		//scene->AddEmpty(555);

		//if(scene->getObjectByName("PlayerLight"))
		//	playerLight = &(scene->getObjectByName("PlayerLight")->getPointLight().value());

		int i = 1;
		Object* enemyObj = nullptr;
		while (enemyObj = scene->getObjectByName("batPrefab" + std::to_string(i))) {
			enemyObjects.push_back(enemyObj);
			enemyObj = nullptr;
			//enemyLights.reserve(1);

			//if(scene->getObjectByName("EnemyLight" + std::to_string(i)))
				//enemyLights.push_back(&(scene->getObjectByName("EnemyLight" + std::to_string(i))->getPointLight().value()));

			i++;	
		}

		instance->debug_msg.append("Objects size: " + enemyObjects.size());

		enemyWalkDirs.resize(enemyObjects.size());

		obj = scene->getObjectByName("Player");
		if (!obj) {
			instance->debug_msg.append("no obj\n");
			return;
		}
		
		obj->unhide();

		//cam = scene->getCameraAt(0);
		cam = &(instance->cam);
		if (!cam)
			instance->debug_msg.append("couldnt ge camera\n");
	}

	void update(ScriptArgument& args) override {
        auto* obj = args.obj;
		auto* scene = args.scene;
		if (!instance || !obj) {
			std::cout << "Iam super duper mega start but i have no instance\n";
			return;
		}

		//println("velocity y ", obj->getRigidBody()->velocity);

		/*if (obj->getRigidBody()->velocity.y < 0) {
			if (p.currAnim != 5) {
				obj->getSpriteAnimator()->setCurrAnim(4);
				p.currAnim = 5;
			}
		}*/

		//obj = scene->get_object_at(2);

		if (instance->ks.get_a() && !instance->ks.get_d() && !instance->ks.get_q() && !instance->ks.get_e())
		{
			if (true || p.currAnim != 2) {
				obj->getSpriteAnimator()->setCurrAnim(3);
				p.currAnim = 2;
			}

			if (!rotatedToLeft) {
				//obj->moveTransform(glm::vec3{ 10 * p.speed, 0, 0 });

				//obj->getTransform().addToPosition({ 10 * p.speed, 0,0 });
				obj->getTransform().rotate(glm::radians(180.0f), glm::vec3{0,1,0});
				rotatedToLeft = true;
			}

			if(rotatedToLeft){
				//obj->getTransform().translatePosition({ p.speed,0,0 });

				obj->getTransform().addToPosition({ -10 * p.speed, 0,0 });
			}

			//obj->getTransform().rotate(glm::radians(180.0f), glm::vec3{0,1,0});

			//obj->getTransform().rotateBy(180, {0,0,1});
			if (cam) {
			//	cam->moveCameraLeft();
				cam->getCameraPosRef() -= glm::vec3{10 * p.speed, 0, 0};
			}
		}

		if(!instance->ks.get_a() && !instance->ks.get_d() && !instance->ks.get_q() && !instance->ks.get_e() && rotatedToLeft)
		{
		//	rotatedToLeft = false;
		//	obj->getTransform().rotate(glm::radians(-180.0f), glm::vec3{0,1,0});
		}

		if (instance->ks.get_d() && !instance->ks.get_a() && !instance->ks.get_q() && !instance->ks.get_e())
		{
			if (true || p.currAnim != 1) {
					
				if (rotatedToLeft) {
					rotatedToLeft = false;
					obj->getTransform().rotate(glm::radians(-180.0f), glm::vec3{0,1,0});
				}

				obj->getSpriteAnimator()->setCurrAnim(3);
				p.currAnim = 1;
			}
			//obj->moveTransform(glm::vec3{ 1 * p.speed, 0, 0 });
			//obj->getTransform().translatePosition({ p.speed, 0,0 });
			obj->getTransform().addToPosition({ 10 * p.speed, 0,0 });

			if (cam) {
				instance->debug_msg.append("moving camera right\n");
				//cam->moveCameraRight();

				cam->getCameraPosRef() += glm::vec3{ 10 * p.speed, 0, 0};
			}
			
		}
        if (instance->ks.get_q() && !instance->ks.get_d() && !instance->ks.get_a() && !instance->ks.get_e() && p.currAnim != 3)
		{
			if (p.currAnim != 3 && p.currAnim != 5) {
				p.inJump = true;
				//instance->debug_msg.append("clicked q");
				obj->getSpriteAnimator()->setCurrAnim(2);

				if (obj->getRigidBody()) {
					obj->getRigidBody()->velocity += glm::vec3({ 0,7,0 });
				}
				p.currAnim = 3;
			}
		}
		if (instance->ks.get_e() && !instance->ks.get_d() && !instance->ks.get_q() && !instance->ks.get_a() && p.currAnim != 4)
		{
			//instance->debug_msg.append("clicked q");

			obj->getSpriteAnimator()->setCurrAnim(1);
			p.currAnim = 4;
		}

		if (!instance->ks.get_d() && !instance->ks.get_a() && !instance->ks.get_q() && !instance->ks.get_e()) {
            if (p.currAnim != 0 && p.currAnim != 5) {
                //obj->setSpriteAnimation(Idle);//NOTE(daius)IMPORTANT(darius) disabled cause of bug in network sync. cause of ansynchronized animation change - bad optional acess

				obj->getSpriteAnimator()->setCurrAnim(0);
                p.currAnim = 0;
            }
        }

		if (instance->ks.get_lshift()) 
		{
			p.speed = 0.003;
		}
		else
		{
			p.speed = 0.002;
		}

		//if(playerLight)
		//	playerLight->position = obj->getTransform().getPosition();

		int i = 0;
		while(i < enemyObjects.size())
		{
			std::time_t currT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			Object* enemyObj = enemyObjects[i];

			if (std::difftime(currT, enemyMotionUpdateTime) > 1) {
				for(int j = 0; j < enemyWalkDirs.size(); j++){
					char randDir = randDirection();

					if (randDir == 'u')
						enemyWalkDirs[j] = glm::vec3{0,enemySpeed,0};

					if (randDir == 'l')
						enemyWalkDirs[j] = { -enemySpeed, 0,0 };

					if (randDir == 'r')
						enemyWalkDirs[j] = { enemySpeed,0,0 };

					if (randDir == 'd')
						enemyWalkDirs[j] = {0, -enemySpeed, 0};
				}

				enemyMotionUpdateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			}

			enemyObj->getTransform().translatePosition(enemyWalkDirs[i]);
			
			if(enemyLights.size() > i && enemyLights[i])
				enemyLights[i]->position = enemyObj->getTransform().getPosition();

			if(obj && obj->getColider() && enemyObjects[i]->getColider())
			{
				if(obj->getColider()->check_collision(*enemyObjects[i]->getColider()) != glm::vec3(0,0,0))
				{
					instance->debug_msg.append("ENEMY COLLISION\n");
				}
			}

			i++;
		}

        //obj->getModel().value().meshes[0] = initMesh;
        //obj->setSpriteAnimation(init);
	}

	void setInstance(GameState* p) override
	{
		instance = p;
	}

	char randDirection()
	{
		return "udlr"[rand() % 4];
	};

	GameState* instance = nullptr;

	Player p;

	std::vector<Object*> enemyObjects;
	std::vector<PointLight*> enemyLights;
	std::vector<glm::vec3> enemyWalkDirs;

	std::time_t enemyMotionUpdateTime = 0;

	float enemySpeed = 0.0001;

	//DANGER(darius) NOTE(darius) you cant create opengl entities, cause u have no opengl initialized here, TODO(darius) make some factory inside engine
	//FlatMesh runMesh;
	//Mesh initMesh;

	PointLight* playerLight = nullptr;
	PointLight* enemyLight = nullptr;

    Camera* cam = nullptr;

	bool rotatedToLeft = false;
};
