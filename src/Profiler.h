#pragma once
#include <vector>
#include <concepts>
#include <string>

#include <Timer.h>

template<typename T> requires std::equality_comparable<T> 
class Profiler
{
public:
	Profiler() = default;

	Profiler(const std::string& name_in) : name(name_in)
	{
	}

	void addRecord(T&& val)
	{
		if(time.checkTime() > dt){
			history.emplace_back(std::move(val));
			time.clearTime();
		}

		if(history.size() > maxHistorySize)
			history.clear();
	}

	size_t size()
	{
		return history.size();
	}

	std::string getNameRef()
	{
		return name;
	}

	std::vector<T>& getHistoryRef()
	{
		return history;
	}

private:
	std::vector<T> history;
	size_t maxHistorySize = 10000;

	std::string name = "";

	double dt = 1;//in secs
	Timer time;
};
