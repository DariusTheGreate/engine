#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <array>

#include <Transform.h>
#include <RigidBody.h>

class Colider;

struct Simplex
{
    //for N 
    union
    {
        //std::array<glm::vec3, 4> points;
        glm::vec3 points[4] = {};
        struct {
            glm::vec3 c;
            glm::vec3 b;
            glm::vec3 a;
            glm::vec3 d;
        };
    };

    size_t sz = 0;
public:
    void push_point(glm::vec3 p)
    {
        points[sz++] = p;
    }

    glm::vec3& last_point()
    {
        assert(sz > 0);
        return points[sz-1];
    }

    glm::vec3& operator [](int i)
    {
        return points[i];
    }

    glm::vec3 crossABA(const glm::vec3& a, const glm::vec3& b)
    {
        return glm::cross(glm::cross(a,b), a);
    }

    bool update(glm::vec3& dir)
    {
        if (sz == 3) {
            update_simplex3(dir);
        }
        else if (update_simplex4(dir)) {
            return true;
        }

        return false;
    }

    void update_simplex3(glm::vec3& dir) {
		glm::vec3 n = glm::cross(b - a, c - a);
        glm::vec3 AO = -a; 

        sz = 2;
        if (glm::dot(glm::cross(b - a, n), AO) > 0) { 
            c = a;
            dir = crossABA(b-a, AO);
            return;
        }
        if (glm::dot(glm::cross(n, c - a), AO) > 0) {
            b = a;
            dir = crossABA(c-a, AO);
            return;
        }

        sz = 3;
        if (glm::dot(n, AO) > 0) { 
            d = c;
            c = b;
            b = a;
            dir = n;
            return;
        }

        d = b;
        b = a;

        dir = -n;
        return;
    }

    /*
    The tetrahedron case is the most complex, but almost entirely made up of triangle cases. We don’t need to test for the origin below the tetrahedron for the same reason as before.
    We only need to determine which face, if any, the origin is in the direction of. If there is one, we’ll go back to the triangle case with that face as the simplex, but if not, 
    we know it must be inside the tetrahedron and we’ll return true.
    */
    bool update_simplex4(glm::vec3& dir) {
        glm::vec3 ABC = glm::cross(b - a, c - a);
        glm::vec3 ACD = glm::cross(c - a, d - a);
        glm::vec3 ADB = glm::cross(d - a, b - a);

        glm::vec3 AO = -a;
        sz = 3;

        if (glm::dot(ABC, AO) > 0) {
            d = c;
            c = b;
            b = a;
            dir = ABC;
            return false;
        }
        if (glm::dot(ACD, AO) > 0) {
            b = a;
            dir = ACD;
            return false;
        }
        if (glm::dot(ADB, AO) > 0) { 
            c = d;
            d = b;
            b = a;
            dir = ADB;
            return false;
        }

        return true;
	}
};

class Colider
{
public:
	Colider(glm::vec3 size_in, Transform& tr_in, RigidBody& rb_in, int tag_in = 0) : size(size_in), tr(tr_in), body(rb_in), tag(tag_in)
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

    std::vector<glm::vec3> get_points() {
        std::vector<glm::vec3> points;
        glm::mat4 transformation = tr.get_quatmat();
        glm::vec4 position = { tr.position.x, tr.position.y, tr.position.z, 0 };

        points[0] = transformation * glm::vec4(size.x, size.y, size.z, 0) + position;
        points[1] = transformation * glm::vec4(size.x, size.y, -size.z, 0) + position;
        points[2] = transformation * glm::vec4(size.x, -size.y, size.z, 0) + position;
        points[3] = transformation * glm::vec4(size.x, -size.y, -size.z, 0) + position;
        points[4] = transformation * glm::vec4(-size.x, size.y, size.z, 0) + position;
        points[5] = transformation * glm::vec4(-size.x, size.y, -size.z, 0) + position;
        points[6] = transformation * glm::vec4(-size.x, -size.y, size.z, 0) + position;
        points[7] = transformation * glm::vec4(-size.x, -size.y, -size.z, 0) + position;

        return points;
    }

    bool contains_point(const glm::vec3& point) {
        glm::mat4 transformation = glm::inverse(body.tr.get_quatmat());
        glm::vec3 v3 = (point - body.tr.position);
        glm::vec3 box_point = transformation * glm::vec4{ v3.x, v3.y, v3.z, 0 };
        glm::vec3 new_half_lengths = size + glm::vec3(0.01, 0.01, 0.01);

        if (box_point.x < -new_half_lengths.x || box_point.x > new_half_lengths.x) {
            return false;
        }

        if (box_point.y < -new_half_lengths.y || box_point.y > new_half_lengths.y) {
            return false;
        }

        if (box_point.z < -new_half_lengths.z || box_point.z > new_half_lengths.z) {
            return false;
        }

        return true;
    }


    glm::vec3 support(glm::vec4 dir) {
        //dir = tr.get_quatmat() * glm::vec4{ tr.scale.x, tr.scale.y, tr.scale.z , 0} * dir; //find support in model space

        glm::vec4 result;
        result.x = (dir.x > 0) ? maxX() : minX();
        result.y = (dir.y > 0) ? maxY() : minY();
        result.z = (dir.z > 0) ? maxZ() : minZ();
        result.w = 0;

        return result;
        //return tr.get_quatmat() * glm::vec4{ tr.scale.x, tr.scale.y, tr.scale.z , 0 } * result;// + glm::vec4{ tr.position.x, tr.position.y, tr.position.z, 0 }; //convert support to world space
    }

    //https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
    bool gjk(Colider* coll1, Colider* coll2) {
        Simplex plex;
        glm::vec3 dir =  coll2->tr.position - coll1->tr.position;

        plex.c = (coll1->support({ dir.x, dir.y, dir.z, 0 }) - coll2->support(glm::vec4{ -dir.x, -dir.y, -dir.z, 0 }));
        if (dot(plex.c, dir) < 0) { return false; }
        dir = -plex.c; 
        plex.sz++;

        plex.b = (coll1->support(glm::vec4{ dir.x, dir.y, dir.z, 0 }) - coll2->support(glm::vec4{ -dir.x, -dir.y, -dir.z, 0 }));
        if (dot(plex.b, dir) < 0) { return false; }
        dir = plex.crossABA(plex.c - plex.b, -plex.b);

        plex.sz++;

        for (int iterations = 0; iterations < 100; iterations++)
        {
            if (dir.length() < 0.0001f)
                return false;

            plex.a = (coll1->support(glm::vec4{ dir.x, dir.y, dir.z, 0 }) - coll2->support(-glm::vec4{ dir.x, dir.y, dir.z, 0 }));
            if (dot(plex.a, dir) < 0) { return false; }

            plex.sz++;

			if (plex.update(dir))
                return true;
        }

        return false;
    }
	
    bool check_collision(const Colider& c) const
	{
		auto cpos = c.get_pos();
		auto csize = c.get_size();

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
		return glm::min(tr.position.x, tr.position.x + size.x);
	}

	float minY() const 
	{
		return glm::min(tr.position.y, tr.position.y + size.y);
	}

	float minZ() const
	{
		return glm::min(tr.position.z, tr.position.z + size.z);
	}

	float maxX() const
	{
		return glm::max(tr.position.x, tr.position.x + size.x);
	}

	float maxY() const
	{
		return glm::max(tr.position.y, tr.position.y + size.y);
	}

	float maxZ() const
	{
		return glm::max(tr.position.z, tr.position.z + size.z);
	}

    glm::vec3 get_render_start_point()
    {
        return glm::vec3{tr.position.x, tr.position.y, tr.position.z};
    }

private:
    //not cool, dangle may happen
	Transform& tr;
    RigidBody& body;
	glm::vec3 size = {1,1,1};
	int tag = 0;
};

