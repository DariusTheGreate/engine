#pragma once

struct Timer {
public:
	Timer(bool printOrNot = false);
	~Timer();

	double checkTime();

	void setBoolPrint(bool p);

	void clearTime();

	double currentTime();

	static void printCurrentTime();

private:
	double timeOfBirth;
	double time;
	bool print = 0;
};
