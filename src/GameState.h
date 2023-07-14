#pragma once
#include <string>

#include "Camera.h"
#include "MouseState.h"
#include "KeyboardState.h"

//TODO(darius) fix this bullshit
class GameState
{
public:
	void msg(std::string&& str);

	void clear_msg();

	static void set_instance(GameState* ptr);

	GameState* get_instance();
	
public:
	static Camera cam;
	static MouseState ms;
	static KeyboardState ks;
	static GameState* instance;
    static int editor_mode;
	static std::string engine_path;

	//TODO(darius) make it a class
	std::string debug_msg;
	size_t debug_len;
};

