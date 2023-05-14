#pragma once

#include <glm/glm.hpp>

#include <iostream>

#include <Transform.h>

class Colider
{
public:
	Colider(Transform& tr_in, int tag_in = 0) : tr(tr_in), tag(tag_in)
	{
	}

	glm::vec3 get_position() const 
	{
		return tr.position;
	}

	glm::vec3 get_size() const 
	{
		return size;
	}

	bool check_collision(const Colider& c) const
	{
		auto cpos = c.get_pos();
		auto csize = c.get_size();

		//std::cout << cpos.x << "|" << cpos.y << "|" << cpos.z << "\n";
		//std::cout << tr.position.x << "|" << tr.position.y << "|" << tr.position.z << "\n";

		//cringe AND Float comparasion UB
		return (minX() <= c.maxX()) && (c.minX() <= maxX()) && (minY() <= c.maxY()) && (c.minY() <= maxY()) && (minZ() <= c.maxZ()) && (c.minZ() <= maxZ());
	}

	glm::vec3 get_pos() const 
	{
		return tr.position;
	}
	
	int get_tag() const
	{
		return tag;
	}

	float minX() const
	{
		return glm::min(tr.position.x + size.x, tr.position.x - size.x);
	}

	float minY() const 
	{
		return glm::min(tr.position.y + size.y, tr.position.y - size.y);
	}

	float minZ() const
	{
		return glm::min(tr.position.z + size.z, tr.position.z - size.z);
	}

	float maxX() const
	{
		return glm::max(tr.position.x + size.x, tr.position.x - size.x);
	}

	float maxY() const
	{
		return glm::max(tr.position.y + size.y, tr.position.y - size.y);
	}

	float maxZ() const
	{
		return glm::max(tr.position.z + size.z, tr.position.z - size.z);
	}

private:
	Transform& tr;
	glm::vec3 size = {0.5,0.5,0.5};
	int tag = 0;
};

