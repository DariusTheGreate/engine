#pragma once

#include <vector>
#include <optional>

class Object;

//NOTE(darius) BATCHES DONT MANAGE MEMORY!! MANAGMENT OF OBJECTS IS FULLY USER RESPONSIBILITY. 
//             MAYBE WILL WORK ON THAT, BUT CURRENTLY I LIKE IT THIS WAY IDK
// ITS IMPORTANT TO CALL clear() after u worked with batch

class Batch
{
public:
	void addObject(Object* obj);

	bool contains(Object* obj);

	std::vector<Object*> takeBack();

	void setOrigin(Object* obj);

	Object* getOrigin();

	size_t size()
	{
		return batched.size();
	}

private:
	Object* origin = nullptr;
	std::vector<Object*> batched;
};

class BatchingCache
{
public:
	void addBatch(Batch&& batch);

	Batch& operator[](int i);

	std::optional<Batch> operator[](Object* obj);

	bool contains(Object* obj);

	bool containsOrigin(Object* obj);

	std::vector<Object*> takeBack();

	std::vector<Object*> takeOrigins();

	std::optional<Batch> takeOrigin(Object* origin);

	size_t size();

	void clearBatch(Object* origin);

	void clear();

private:
	std::vector<Batch> batches;
};
