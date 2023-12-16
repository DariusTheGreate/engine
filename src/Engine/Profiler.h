#pragma once
#include <vector>
#include <concepts>
#include <string>
#include <fstream>

#include <Core/Timer.h>

template<typename T> requires std::equality_comparable<T> 
class Profiler
{
public:
	Profiler() = default;

	Profiler(const std::string& name_in) : name(name_in)
	{
		startt = time.currentTime();
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

	std::string& getNameRef()
	{
		return name;
	}

	void setName(std::string name_in)
	{
		name = name_in;
	}

	std::vector<T>& getHistoryRef()
	{
		return history;
	}

	void serialize(std::string&& path)
	{
		std::ofstream file(path.data());

	   	if(!file.is_open())
    		return;

		for(int i = 0; i < history.size(); ++i)
		{
			file << "TimeStamp: " << startt + i * dt << " | " << "Value: " << history[i] << "\n";//NOTE(darius) not generic for << T 
		}

	    file.close();
	}

private:
	std::vector<T> history;
	size_t maxHistorySize = 10000;

	std::string name = "";

	double dt = 1;//in secs
	double startt = 0;

	Timer time;
};
