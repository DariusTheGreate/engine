#pragma once
#include <OpenglWrapper.h>

#include <string>

class SystemInfo 
{
public:
    SystemInfo();

	std::string_view getCPU();

	std::string_view getGPU();

	static void setInfo(SystemInfo* v);

	static SystemInfo* getInfo();

private:
	std::string cpuName;
	std::string gpuName;

	int mainMemory = 0;
	int videoMemory = 0;

	static SystemInfo* value;
};

