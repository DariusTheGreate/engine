#pragma once
#include <string>

#include "Camera.h"
#include "MouseState.h"
#include "KeyboardState.h"

class GameState
{
public:
	static void msg(std::string&& str)
	{
		if (debug_msg.size() >= debug_len)
			clear_msg();
		debug_msg.append(str);
	}

	static void clear_msg()
	{
		debug_msg.clear();
	}
	
public:
	static Camera cam;
	static MouseState ms;
	static KeyboardState ks;

	//TODO(darius) make it a class
	static std::string debug_msg;
	static size_t debug_len;
};

