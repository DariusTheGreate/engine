#pragma once
#include <OpenglWrapper.h>

#include <string>

#include "windows.h"
#include "psapi.h"

struct MemInfo
{
	DWORDLONG totalVirtualMem;
	DWORDLONG virtualMemUsed;
	DWORDLONG  physMemUsed;

	SIZE_T virtualMemUsedByMe;
	SIZE_T physMemUsedByMe ;
};

class SystemInfo 
{
public:
    SystemInfo();

	std::string_view getCPU();

	std::string_view getGPU();

	bool isOcclusionCullingAvailable();

	static void setInfo(SystemInfo* v);

	static SystemInfo* getInfo();

	MemInfo getMemoryInfo();

private:
	std::string cpuName;
	std::string gpuName;

	bool occlusionCullingPresentedOnThisDevice = false;

	int mainMemory = 0;
	int videoMemory = 0;

	static SystemInfo* value;
};

