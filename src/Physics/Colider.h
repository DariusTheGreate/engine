#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <array>

struct Simplex;
struct Transform;

constexpr float collisionEpsilon = 0.000000000000000001f;

class Colider
{
public:
    Colider(const glm::vec3& size_in, Transform& tr_in, int tag_in = 0, bool active_in = true);

    glm::vec3 get_position() const;

    glm::vec3 get_size() const;
	void set_size(const glm::vec3& v);

	glm::vec3 get_shift() const;
	void set_shift(const glm::vec3& v);

    glm::vec3& get_size_ref();

    std::vector<glm::vec3> get_points();

    bool contains_point(const glm::vec3& point);

	glm::vec3 support(const glm::vec4& dir);
	glm::vec3 dotSupport(const glm::vec4& dir);
	glm::vec3 supportOriented(const glm::vec4& dir);

    //https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
	bool gjk(Colider* coll1, Colider* coll2);

	glm::vec3 check_collision(const Colider& c) const;

	glm::vec3 get_pos() const;

	int get_tag() const;

	float minX() const;

	float minY() const;

	float minZ() const;

	float maxX() const;

	float maxY() const;

	float maxZ() const;

	glm::vec3 get_render_start_point() const;

	glm::vec3& get_render_shift();

	glm::vec3 colider_position() const;

	glm::vec3 get_epa();
	
	bool is_active() const;
	bool* activeAdr();//TODO(darius) make it macro UICONTROL(active) or smthng
	bool* get_collision_state();

	Transform& get_transform();

public:
	glm::vec4 collider_debug_color = {0, 1, 0, 0};

private:
    //not cool, dangle may happen
	Transform& tr;
	glm::vec3 size = {1,1,1};
	bool active = true;
	int tag = 0;
	bool collision_state = false;
	glm::vec3 shift = {0,0,0};

	glm::vec3 epa_collision_value = { 0,0,0 };
};
