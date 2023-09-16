#include "Timer.h"

Timer::Timer(bool printOrNot) : print(printOrNot) {
	time = 0.0;
	timeOfBirth = glfwGetTime();
}

Timer::~Timer() {
	if (print) {
		std::cout << checkTime() << "\n";
	}
}

double Timer::checkTime() {
	time = glfwGetTime() - timeOfBirth;
	return time;
}

void Timer::clearTime() {
	timeOfBirth = glfwGetTime();
}

void Timer::setBoolPrint(bool p) {
	print = p;
}
