#include <iostream>

#include "Application.h"

int main()
{
	std::cout << "------\n";
	Application* app = new Application((char*)"some app", 1, 1080, 720);
	app->Run();
	std::cout << "------\n";
	return 0;
}
