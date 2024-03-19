#include "Core/Timer.h"
#include <Core/Printer.h>

//TODO: why chrono AND glfw? 
#include <chrono>
#include <ctime>
#include <array>
#include <GLFW/glfw3.h>

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

double Timer::currentTime()
{
	return glfwGetTime();
}

void Timer::printCurrentTime()
{
    auto currTime = std::chrono::system_clock::now();	
    auto timeFormatted = std::chrono::system_clock::to_time_t(currTime);

    std::array<char, 26> formatBuff;
    ctime_s(formatBuff.data(), 26, &timeFormatted);
	println("Time: ", formatBuff.data());
}

