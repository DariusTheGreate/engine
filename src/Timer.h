#pragma once
#include <iostream>
#include <GLFW/glfw3.h>

struct Timer {
public:
	Timer();
	~Timer();

	double checkTime();

	void setBoolPrint(bool p);

	void clearTime();
	
private:
	double timeOfBirth;
	double time;
	bool print = 1;
};