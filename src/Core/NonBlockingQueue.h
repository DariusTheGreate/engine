#pragma once

#include <boost/lockfree/queue.hpp>
#include <iostream>

#include <Engine/StackTrace.h>

//TODO(darisu) finish
template<typename T>
class NonBlockingQueue
{
public:
    bool Push(T value) 
    {
		try {
		    return buffer.bounded_push(std::move(value));
		} catch (...) {
			std::cout << "EXCEPTION: ";
			StackTrace::printCurrentStackTrace();
		    return false;
		}
    }

    T Take()
    {
		T value;
		bool success = buffer.pop(value);
		return value;
    }

    size_t size()
    {
        //TODO(darius) bruh..
    	return 0;//buffer.size();
    }

    size_t Clear() 
    {
		while(Take());
    }

private:
	using lockfreeQueue = boost::lockfree::queue<T>;
    lockfreeQueue buffer;
};
