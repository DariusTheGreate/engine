#pragma once

#include <ScriptApi.h>
#include <GameState.h>
#include <Object.h>
#include <Animation.h>
#include <KeyboardState.h>
#include <Printer.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>
#include <chrono>

struct Player 
{
	float speed = 1.0f;
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

		instance->debug_msg.append("Start Script..");
        
        auto* obj = args.obj;
		auto* scene = args.scene;
		//obj->addPointLight();//wont affect lighting 
		//obj->frozeObject();

		//scene->AddEmpty(555);

		/*run = SpriteAnimation(1, 8, 100);
		*run.getBorder() = 0.5;
		run.initPoints();
		init = SpriteAnimation(1,6,100);
		*init.getBorder() = 0.8f;
		init.initPoints();
		dance = SpriteAnimation(1,9,100);
		*dance.getLength() = 14;
		*dance.getBorder() = 0.3f;
		dance.initPoints();
		*/


		/*if (scene->getObjectByName("UpAnim")) {

			WalkUpMesh = &scene->getObjectByName("UpAnim")->getModel()->meshes[0];

			WalkUp = *scene->getObjectByName("UpAnim")->getSpriteAnimation();
			instance->debug_msg.append("up loaded\n");
			std::cout << "Up loaded..\n";
		}

		if (scene->getObjectByName("DownAn")) {
			WalkDownMesh = &scene->getObjectByName("DownAn")->getModel()->meshes[0];

			WalkDown = *scene->getObjectByName("DownAn")->getSpriteAnimation();
			instance->debug_msg.append("down loaded\n");

			std::cout << "loaded..\n";
		}*/

		if (scene->getObjectByName("runAnimation")) {
			WalkSideMesh = &scene->getObjectByName("runAnimation")->getModel()->meshes[0];
			WalkSide = *scene->getObjectByName("runAnimation")->getSpriteAnimation();
			instance->debug_msg.append("runAnimation loaded\n");

			std::cout << "loaded..\n";
		}

		if (scene->getObjectByName("idleAnimation")) {
			IdleMesh = &scene->getObjectByName("idleAnimation")->getModel()->meshes[0];
			Idle = *scene->getObjectByName("idleAnimation")->getSpriteAnimation();
			instance->debug_msg.append("idleAnimation loaded\n");

			std::cout << "idle loaded..\n";
		}

		if (scene->getObjectByName("jumpAnimation")) {
			JumpMesh = &scene->getObjectByName("jumpAnimation")->getModel()->meshes[0];
			Jump = *scene->getObjectByName("jumpAnimation")->getSpriteAnimation();
			instance->debug_msg.append("jumpAnimation loaded\n");

			std::cout << "loaded..\n";
		}
		if (scene->getObjectByName("attackAnimation")) {
			AttackMesh = &scene->getObjectByName("attackAnimation")->getModel()->meshes[0];
			Attack = *scene->getObjectByName("attackAnimation")->getSpriteAnimation();
			instance->debug_msg.append("attackAnimation loaded\n");

			std::cout << "loaded..\n";
		}

		//if(scene->getObjectByName("PlayerLight"))
		//	playerLight = &(scene->getObjectByName("PlayerLight")->getPointLight().value());

		/*int i = 0;
		Object* enemyObj = nullptr;
		while (enemyObj = scene->getObjectByName("EnemyWalkingAnim" + std::to_string(i))) {
			enemyObjects.push_back(enemyObj);
			enemyObj = nullptr;
			//enemyLights.reserve(1);

			if(scene->getObjectByName("EnemyLight" + std::to_string(i)))
				enemyLights.push_back(&(scene->getObjectByName("EnemyLight" + std::to_string(i))->getPointLight().value()));

			i++;	
		}

		enemyWalkDirs.resize(enemyObjects.size());
		*/

		obj = scene->getObjectByName("Player");
		if (!obj) {
			instance->debug_msg.append("no obj\n");
			return;
		}
		
		obj->unhide();

		obj->setSpriteAnimation(Idle);

		//cam = scene->getCameraAt(0);
		cam = &(instance->cam);
		if (!cam)
			instance->debug_msg.append("couldnt ge camera\n");
		//else
		//	*(cam->getCameraSpeed()) = p.speed * 10;

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

		if (instance->ks.get_a() && !instance->ks.get_d() && !instance->ks.get_q() && !instance->ks.get_e())
		{
			if (p.currAnim != 2) {
				obj->getModel()->meshes[0] = *WalkSideMesh;
				obj->setSpriteAnimation(WalkSide);
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
			if (p.currAnim != 1) {
					
				if (rotatedToLeft) {
					rotatedToLeft = false;
					obj->getTransform().rotate(glm::radians(-180.0f), glm::vec3{0,1,0});
				}

				obj->getModel()->meshes[0] = *WalkSideMesh;
				obj->setSpriteAnimation(WalkSide);
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
			if (p.currAnim != 3) {
				//instance->debug_msg.append("clicked q");
				obj->getModel()->meshes[0] = *JumpMesh;
				obj->setSpriteAnimation(Jump);
				if (obj->getRigidBody())
					obj->getRigidBody()->apply_impulse({ 0,15 * p.speed,0 });
				p.currAnim = 3;
			}
		}
		if (instance->ks.get_e() && !instance->ks.get_d() && !instance->ks.get_q() && !instance->ks.get_a() && p.currAnim != 4)
		{
			//instance->debug_msg.append("clicked q");
			obj->getModel()->meshes[0] = *AttackMesh;
			obj->setSpriteAnimation(Attack);
			p.currAnim = 4;
		}
        /*if (instance->ks.get_w())
        {
            if (p.currAnim != 1) {
                obj->getModel()->meshes[0] = *WalkUpMesh;
                obj->setSpriteAnimation(WalkUp);
                p.currAnim = 1;
            }

			//std::cout << "pos " << obj->getTransform().getPosition().x << " " << obj->getTransform().getPosition().y << " " << obj->getTransform().getPosition().z;
            //obj->moveTransform(glm::vec3{ 0, 0, -1*p.speed});
			//std::cout << "pos after " << obj->getTransform().getPosition().x << " " << obj->getTransform().getPosition().y << " " << obj->getTransform().getPosition().z;

			glm::vec3 pos = obj->getTransform().getPosition();
			//pos += glm::vec3{ 0, 0, -1 * p.speed };
			if(rotatedToLeft)
				obj->getTransform().translatePosition({0,0,-p.speed});
			else
				obj->getTransform().translatePosition({0,0,p.speed});
        }
        if(instance->ks.get_s())
        {
            if (p.currAnim != 1) {
                obj->getModel()->meshes[0] = *WalkDownMesh;
                obj->setSpriteAnimation(WalkDown);
                p.currAnim = 1;
            }
            //obj->moveTransform(glm::vec3{ 0, 0, 1*p.speed });
			if(rotatedToLeft)
				obj->getTransform().translatePosition({0,0,p.speed});
			else
				obj->getTransform().translatePosition({0,0,-p.speed});
        }*/
        if(!instance->ks.get_d() && !instance->ks.get_a() && !instance->ks.get_q() && !instance->ks.get_e()){
            if (p.currAnim != 0) {
                obj->getModel()->meshes[0] = *IdleMesh;
                obj->setSpriteAnimation(Idle);//NOTE(daius)IMPORTANT(darius) disabled cause of bug in network sync. cause of ansynchronized animation change - bad optional acess
                p.currAnim = 0;
            }
        }

		if (instance->ks.get_lshift()) 
		{
			p.speed = 0.003;
			WalkUp.setDelay(70);
			WalkDown.setDelay(70);
			WalkSide.setDelay(70);
		}
		else
		{
			p.speed = 0.002;

			WalkUp.setDelay(100);
			WalkDown.setDelay(100);
			WalkSide.setDelay(100);
		}

		//if(playerLight)
		//	playerLight->position = obj->getTransform().getPosition();

		/*int i = 0;
		while(i < enemyObjects.size())
		{
			std::time_t currT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			Object* enemyObj = enemyObjects[i];

			if (std::difftime(currT, enemyMotionUpdateTime) > 1) {
				for(int j = 0; j < enemyWalkDirs.size(); j++){
					char randDir = randDirection();

					if (randDir == 'u')
						enemyWalkDirs[j] = glm::vec3{0,0, enemySpeed};

					if (randDir == 'l')
						enemyWalkDirs[j] = { -enemySpeed, 0,0 };

					if (randDir == 'r')
						enemyWalkDirs[j] = { enemySpeed,0,0 };

					if (randDir == 'd')
						enemyWalkDirs[j] = {0,0, -enemySpeed};
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
		*/

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
	SpriteAnimation WalkUp;
	SpriteAnimation Attack;
	SpriteAnimation Jump;
	SpriteAnimation WalkDown;
	SpriteAnimation WalkSide;
	SpriteAnimation Idle;

	SpriteAnimation EnemyWalkSide;

	std::vector<Object*> enemyObjects;
	std::vector<PointLight*> enemyLights;
	std::vector<glm::vec3> enemyWalkDirs;

	std::time_t enemyMotionUpdateTime = 0;

	float enemySpeed = 0.01;

	//DANGER(darius) NOTE(darius) you cant create opengl entities, cause u have no opengl initialized here, TODO(darius) make some factory inside engine
	//FlatMesh runMesh;
	//Mesh initMesh;

	Mesh* WalkUpMesh = nullptr;
	Mesh* AttackMesh = nullptr;
	Mesh* JumpMesh = nullptr;
	Mesh* WalkDownMesh = nullptr;
	Mesh* WalkSideMesh = nullptr;
	Mesh* IdleMesh = nullptr;

	Mesh* EnemyWalkSideMesh = nullptr;

	PointLight* playerLight = nullptr;
	PointLight* enemyLight = nullptr;

    Camera* cam = nullptr;

	bool rotatedToLeft = false;

};