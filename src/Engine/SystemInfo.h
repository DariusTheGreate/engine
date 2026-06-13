#pragma once
//#include <OpenglWrapper.h>

#include <string>
#include <cstdint>
#include <cstddef>

#ifdef _WIN32
#include "windows.h"
#include "psapi.h"
#endif

struct MemInfo
{
	uint64_t totalVirtualMem = 0;
	uint64_t virtualMemUsed = 0;
	uint64_t physMemUsed = 0;

	size_t virtualMemUsedByMe = 0;
	size_t physMemUsedByMe = 0;
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

