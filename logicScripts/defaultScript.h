#pragma once

#include <ScriptApi.h>
#include <GameState.h>
#include <Object.h>
#include <Animation.h>
#include <KeyboardState.h>

#include <boost/config.hpp> 
#include <iostream>
#include <string>
#include <chrono>

struct Player 
{
	float speed = 0.01f;
	int currAnim = 0;//0 - idle; 1 - runR; 2 - run; 3 - dance
};

class DefaultScript : public ScriptRoutine {
public:
	void start(ScriptArgument& args) override {
		instance->debug_msg.append("Enter Script..");
		std::cout << "Enter Scrtipt..\n";
		//return;

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

		if (scene->getObjectByName("UpAnim")) {
			WalkUpMesh = &scene->getObjectByName("UpAnim")->getModel()->meshes[0];

			WalkUp = *scene->getObjectByName("UpAnim")->getSpriteAnimation();
			instance->debug_msg.append("up loaded");
		}

		if (scene->getObjectByName("DownAn")) {
			WalkDownMesh = &scene->getObjectByName("DownAn")->getModel()->meshes[0];

			WalkDown = *scene->getObjectByName("DownAn")->getSpriteAnimation();
			instance->debug_msg.append("down loaded");
		}

		if (scene->getObjectByName("SideAn")) {
			WalkSideMesh = &scene->getObjectByName("SideAn")->getModel()->meshes[0];
			WalkSide = *scene->getObjectByName("SideAn")->getSpriteAnimation();
			instance->debug_msg.append("side loaded");
		}

		if (scene->getObjectByName("IdleAn")) {
			IdleMesh = &scene->getObjectByName("IdleAn")->getModel()->meshes[0];
			Idle = *scene->getObjectByName("IdleAn")->getSpriteAnimation();
			instance->debug_msg.append("idle loaded");
		}

		if(scene->getObjectByName("PlayerLight"))
			playerLight = &(scene->getObjectByName("PlayerLight")->getPointLight().value());

		int i = 0;
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

		obj = scene->getObjectByName("Play");
		if (!obj) {
			instance->debug_msg.append("no obj");
			return;
		}
		
		obj->unhide();

		obj->setSpriteAnimation(Idle);


		cam = scene->getCameraAt(0);
		//if (!cam)
		//	instance->debug_msg.append("couldnt ge camera");
		//else
		//	*cam->getCameraSpeed() = 0.5f;

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

		if (instance->ks.get_a()) 
		{
			if (p.currAnim != 2) {
				obj->getModel()->meshes[0] = *WalkSideMesh;
				obj->setSpriteAnimation(WalkSide);
				p.currAnim = 2;
			}

			if (!rotatedToLeft) {
				obj->moveTransform(glm::vec3{ 1 * p.speed, 0, 0 });
				obj->getTransform().rotate(glm::radians(180.0f), glm::vec3{0,1,0});
				rotatedToLeft = true;
			}

			if(rotatedToLeft){
				obj->getTransform().translatePosition({ p.speed,0,0 });
			}

			//obj->getTransform().rotate(glm::radians(180.0f), glm::vec3{0,1,0});

			//obj->getTransform().rotateBy(180, {0,0,1});
			if (cam)
				cam->moveCameraLeft();
		}

		if(!instance->ks.get_a() && rotatedToLeft)
		{
			rotatedToLeft = false;
			obj->getTransform().rotate(glm::radians(-180.0f), glm::vec3{0,1,0});
		}

		if (instance->ks.get_d())
		{
			if (p.currAnim != 1) {
				obj->getModel()->meshes[0] = *WalkSideMesh;
				obj->setSpriteAnimation(WalkSide);
				p.currAnim = 1;
			}
			//obj->moveTransform(glm::vec3{ 1 * p.speed, 0, 0 });
			obj->getTransform().translatePosition({ p.speed, 0,0 });
			if (cam)
				cam->moveCameraRight();
		}
        if (instance->ks.get_q()) 
		{
            //instance->debug_msg.append("clicked q");
			obj->getModel()->meshes[0] = *WalkUpMesh;
			obj->setSpriteAnimation(WalkUp);
			if(obj->getRigidBody())
				obj->getRigidBody()->apply_impulse({0,15*p.speed,0});
			p.currAnim = 3;
		}
        if(instance->ks.get_w())
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
        }
        if(!instance->ks.get_q() && !instance->ks.get_d() && !instance->ks.get_a() && !instance->ks.get_w() && !instance->ks.get_s()){
            //if (p.currAnim != 0) {
                obj->getModel()->meshes[0] = *IdleMesh;
                obj->setSpriteAnimation(Idle);
                p.currAnim = 0;
            //}
        }

		if (instance->ks.get_lshift()) 
		{
			p.speed = 0.03;
			WalkUp.setDelay(6);
			WalkDown.setDelay(6);
			WalkSide.setDelay(6);
		}
		else
		{
			p.speed = 0.02;

			WalkUp.setDelay(100);
			WalkDown.setDelay(100);
			WalkSide.setDelay(100);
		}

		if(playerLight)
			playerLight->position = obj->getTransform().getPosition();

		int i = 0;
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
	SpriteAnimation WalkUp;
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
	Mesh* WalkDownMesh = nullptr;
	Mesh* WalkSideMesh = nullptr;
	Mesh* IdleMesh = nullptr;

	Mesh* EnemyWalkSideMesh = nullptr;

	PointLight* playerLight = nullptr;
	PointLight* enemyLight = nullptr;

    Camera* cam = nullptr;

	bool rotatedToLeft = false;

};