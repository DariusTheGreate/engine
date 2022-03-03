#pragma once
#include "Camera.h"
#include "MouseState.h"
#include "KeyboardState.h"

class GameState
{
public:
public:
	//static not cool. Anyone can chnge state of anythin
	static Camera cam;
	static MouseState ms;
	static KeyboardState ks;
};


