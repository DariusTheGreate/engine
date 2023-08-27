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

	static void setEditorCameraMode();

	static void saveActiveCameraStateIntoEditorCamera();
	
public:
	//TODO(darius) make it not static? See why below
	static Camera cam;// active camera
	static Camera* editorCamera;
	static bool editorCameraMode;

	static float gammaBrightness;
	static float gammaFactor;

	static MouseState ms;
	KeyboardState ks;
	static GameState* instance;
    static int editor_mode;
	static std::string engine_path;

	//NOTE(darius) nevermind it for now. I used that to test connection between scriptApi and engine app. We need to store objects, not static states. 
	//Cause when logicScipt reads static field from instacne it actually reads its own state(one that present in engine.lib, that logicScripts uses)

	int connect = 228;

	//TODO(darius) make it a class
	std::string debug_msg;
	size_t debug_len;
};

