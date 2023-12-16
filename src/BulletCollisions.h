#pragma once

#include <btBulletDynamicsCommon.h>

constexpr inline const unsigned int maxBulletObjects = 16000; 

class BulletCollisions
{
public:
	BulletCollisions()
	{
		btCollisionConfiguration =  new btDefaultCollisionConfiguration();
		btDispatcher = new btCollisionDispatcher(btCollisionConfiguration);
		btBroadhase = new btDbvtBroadphase();
		//btBroadhase = new bt32BitAxisSweep3(worldAabbMin, worldAabbMax, maxBulletObjects, 0, true);
		btCollisionWorld = new btCollisionWorld(btDispatcher, btBroadhase, btCollisionConfiguration);
	}

private:	
	btCollisionConfiguration* btCollisionConfiguration = nullptr;
	btCollisionDispatcher* btDispatcher = nullptr;
	btBroadphaseInterface* btBroadhase = nullptr;
	btCollisionWorld* btCollisionWorld = nullptr;
};
