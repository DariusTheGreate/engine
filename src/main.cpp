#include <iostream>

#include "Application.h"
#include "GameState.h"

Camera GameState::cam;
MouseState GameState::ms;
KeyboardState GameState::ks;

int main()
{
	std::cout << "------\n";
	Application* app = new Application((char*)"some app", 1080, 720);
	app->Run();
	std::cout << "------\n";
	return 0;
}
