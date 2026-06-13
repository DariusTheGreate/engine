#include <Engine/SystemInfo.h>
#include <Rendering/OpenglWrapper.h>
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#include <intrin.h>
#include <psapi.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#elif defined(__linux__)
#include <cstdio>
#include <cstring>
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

SystemInfo::SystemInfo()
{
	char cpuNameRaw[64] = "";

#if defined(_WIN32) && (defined(_M_X64) || defined(_M_IX86))
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
#elif defined(__APPLE__)
	size_t cpuNameLen = sizeof(cpuNameRaw);
	if (sysctlbyname("machdep.cpu.brand_string", cpuNameRaw, &cpuNameLen, nullptr, 0) != 0)
		cpuNameRaw[0] = '\0';
#elif defined(__linux__)
	if (FILE* f = std::fopen("/proc/cpuinfo", "r"))
	{
		char line[256];
		while (std::fgets(line, sizeof(line), f))
		{
			if (std::strncmp(line, "model name", 10) == 0)
			{
				const char* colon = std::strchr(line, ':');
				if (colon)
				{
					const char* val = colon + 1;
					while (*val == ' ' || *val == '\t') ++val;
					std::strncpy(cpuNameRaw, val, sizeof(cpuNameRaw) - 1);
					cpuNameRaw[std::strcspn(cpuNameRaw, "\n")] = '\0';
				}
				break;
			}
		}
		std::fclose(f);
	}
#endif

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

#if defined(_WIN32)
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
#elif defined(__APPLE__)
	uint64_t totalPhys = 0;
	size_t len = sizeof(totalPhys);
	if (sysctlbyname("hw.memsize", &totalPhys, &len, nullptr, 0) != 0)
		totalPhys = 0;
	memoryInformation.totalVirtualMem = totalPhys;

	vm_size_t pageSize = 0;
	mach_port_t host = mach_host_self();
	host_page_size(host, &pageSize);

	vm_statistics64_data_t vmStats;
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vmStats, &count) == KERN_SUCCESS)
	{
		uint64_t used = (uint64_t)(vmStats.active_count + vmStats.inactive_count + vmStats.wire_count) * pageSize;
		memoryInformation.physMemUsed = used;
		memoryInformation.virtualMemUsed = used;
	}

	struct mach_task_basic_info taskInfo;
	mach_msg_type_number_t taskCount = MACH_TASK_BASIC_INFO_COUNT;
	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&taskInfo, &taskCount) == KERN_SUCCESS)
	{
		memoryInformation.physMemUsedByMe = taskInfo.resident_size;
		memoryInformation.virtualMemUsedByMe = taskInfo.virtual_size;
	}
#elif defined(__linux__)
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		const uint64_t unit = info.mem_unit ? info.mem_unit : 1;
		memoryInformation.totalVirtualMem = (uint64_t)(info.totalram + info.totalswap) * unit;
		memoryInformation.virtualMemUsed =
			(uint64_t)((info.totalram - info.freeram) + (info.totalswap - info.freeswap)) * unit;
		memoryInformation.physMemUsed = (uint64_t)(info.totalram - info.freeram) * unit;
	}

	// Per-process resident/virtual size from /proc/self/statm (pages).
	if (FILE* f = std::fopen("/proc/self/statm", "r"))
	{
		unsigned long vmPages = 0, rssPages = 0;
		if (std::fscanf(f, "%lu %lu", &vmPages, &rssPages) == 2)
		{
			const long pageSize = sysconf(_SC_PAGESIZE);
			memoryInformation.virtualMemUsedByMe = (size_t)vmPages * pageSize;
			memoryInformation.physMemUsedByMe = (size_t)rssPages * pageSize;
		}
		std::fclose(f);
	}
#endif

	return memoryInformation;
}

SystemInfo* SystemInfo::value = nullptr;
