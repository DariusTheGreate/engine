#include <BatchingCache.h>

void Batch::addObject(Object* obj)
{
	batched.push_back(obj);
}

bool Batch::contains(Object* obj)
{
	return std::find(batched.begin(), batched.end(), obj) != batched.end();
}	

std::vector<Object*> Batch::takeBack()
{
	return std::move(batched);
}

void Batch::setOrigin(Object* obj)
{
	origin = obj;
}

Object* Batch::getOrigin()
{
	return origin;
}

void BatchingCache::addBatch(Batch&& batch)
{
	batches.emplace_back(std::move(batch));
}

Batch& BatchingCache::operator[](int i)
{
	return batches[i];
}

std::optional<Batch> BatchingCache::operator[](Object* obj)
{
	for(auto& v : batches)
	{
		if(v.contains(obj))
			return v;
	}

	return {};
}

bool BatchingCache::contains(Object* obj)
{
	for(auto& b : batches)
	{
		if(b.contains(obj))
			return true;
	}

	return false;
}

bool BatchingCache::containsOrigin(Object* obj)
{
	for(auto& v : batches)
	{
		if(v.getOrigin() == obj)
			return true;
	}

	return false;
}

std::vector<Object*> BatchingCache::takeBack()
{
	std::vector<Object*> result;		
	for(auto& v : batches)
	{
		auto batched = v.takeBack();
		result.insert(result.end(), batched.begin(), batched.end());
	}

	return (result);
}

std::vector<Object*> BatchingCache::takeOrigins()
{
	std::vector<Object*> origins;
	for(auto& v : batches)
	{
		if(v.getOrigin())
			origins.push_back(v.getOrigin());	
	}

	return origins;
}

std::optional<Batch> BatchingCache::takeOrigin(Object* origin)
{
	for(auto& v : batches)
	{
		if(v.getOrigin() == origin)
			return v;	
	}	

	return {};
}

size_t BatchingCache::size()
{
	size_t resSz = 0;

	for(auto& b : batches)
	{
		resSz += b.size();
	}

	return resSz;
}

void BatchingCache::clearBatch(Object* origin)
{
	for(auto it = batches.begin(); it < batches.end(); ++it)
	{
		if(it->getOrigin() == origin)
		{
			it = batches.erase(it);
		}
	}	
}

void BatchingCache::clear()
{
	batches.clear();
}
