#include <SystemInfo.h>

#include <GLFW/glfw3.h>
#include <intrin.h>

SystemInfo::SystemInfo()
{
	char cpuNameRaw[64] = "";

	int cpuInfo[4] = { -1 };
	__cpuid(cpuInfo, 0x80000000);
	int nExIds = cpuInfo[0];
	for (int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(cpuInfo, i);
		if (i == 0x80000002)
		{
			memcpy(cpuNameRaw,
				cpuInfo,
				sizeof(cpuInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(cpuNameRaw + 16,
				cpuInfo,
				sizeof(cpuInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(cpuNameRaw + 32, cpuInfo, sizeof(cpuInfo));
		}
	}

	cpuName = cpuNameRaw;

	std::string_view vendor = OpenglWrapper::GetString(GL_VENDOR);
	std::string_view model = OpenglWrapper::GetString(GL_RENDERER);

	gpuName = std::string(vendor);
	gpuName.append(model);

    GLint bitsSupported;
	glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &bitsSupported);
    if (bitsSupported == 0) {
    	occlusionCullingPresentedOnThisDevice = false;
    }
    else
    {
    	occlusionCullingPresentedOnThisDevice = true;
    }
}

std::string_view SystemInfo::getCPU()
{
	return cpuName;
}

std::string_view SystemInfo::getGPU()
{
	return gpuName;
}

void SystemInfo::setInfo(SystemInfo* v)
{
	value = v;
}

SystemInfo* SystemInfo::getInfo()
{
	return value;
}

bool SystemInfo::isOcclusionCullingAvailable()
{
	return occlusionCullingPresentedOnThisDevice;
}

MemInfo SystemInfo::getMemoryInfo()
{
	MemInfo memoryInformation;

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	memoryInformation.totalVirtualMem = memInfo.ullTotalPageFile;
	memoryInformation.virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	memoryInformation.physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;	

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	memoryInformation.virtualMemUsedByMe = pmc.PrivateUsage;
	memoryInformation.physMemUsedByMe  = pmc.WorkingSetSize;

	return memoryInformation;
}

SystemInfo* SystemInfo::value = nullptr;
