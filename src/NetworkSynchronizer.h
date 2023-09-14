#pragma once

#include <BlockingQueue.h>
#include <NonBlockingQueue.h>

class Object;

//TODO(darius) make it template, so u can go NetSync<Trnasform> , NEtSync<string>, NetSync<pointLight>, etc
class NetworkSynchronizer : public BlockingQueue<Object*> //public NonBLockingQueue<Object*>//public BlockingQueue<Object*>
{
public:
	void pushObject(Object* obj)
	{
		this->Push(obj);
	}

	Object* getObject()
	{
		Object* obj = this->Take();//NOTE(darius) exceptions and stuff duh
		return obj;
	}

private:
};
