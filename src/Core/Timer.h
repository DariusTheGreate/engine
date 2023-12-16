#pragma once
#include <iostream>
#include <GLFW/glfw3.h>

struct Timer {
public:
	Timer(bool printOrNot = false);
	~Timer();

	double checkTime();

	void setBoolPrint(bool p);

	void clearTime();

	double currentTime();

private:
	double timeOfBirth;
	double time;
	bool print = 0;
};