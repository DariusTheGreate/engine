#pragma once
#include <string>

#include "Camera.h"
#include "MouseState.h"
#include "KeyboardState.h"

class GameState
{
public:
	void msg(std::string&& str)
	{
		if (debug_msg.size() >= debug_len)
			clear_msg();
		debug_msg.append(str);
	}

	void clear_msg()
	{
		debug_msg.clear();
	}

	static void set_instance(GameState* ptr) 
	{
		GameState::instance = ptr;
	}

	GameState* get_instance() 
	{
		return instance;
	}
	
public:
	static Camera cam;
	static MouseState ms;
	static KeyboardState ks;
	static GameState* instance;

	//TODO(darius) make it a class
	std::string debug_msg;
	size_t debug_len;
};

