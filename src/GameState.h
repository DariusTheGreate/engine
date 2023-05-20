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
		debug_msg.append(str);
	}
public:
	static Camera cam;
	static MouseState ms;
	static KeyboardState ks;
	static std::string debug_msg;
};

