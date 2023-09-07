#pragma once
#include <vector>

class Object;

class Batch
{
public:
	void addObject(Object* obj)
	{
		batched.push_back(obj);
	}

	bool contains(Object* obj)
	{
		return std::find(batches.begin(), batches.end(), obj) != batches.end();
	}	

private:
	std::vector<Object*> batched;
};

class BatchingCache
{
public:
	void addBatch(Batch&& batch)
	{
		batches.emplace_back(std::move(batch));
	}

	Batch& operator[](int i)
	{
		return batches[i];
	}

	bool contains(Object* obj)
	{
		for(auto& b : batches)
		{
			b.contains(obj);
		}
	}

private:
	std::vector<Batch> batches;
};
