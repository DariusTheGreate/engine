#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp >
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

#include <Transform.h>

class RigidBody
{
public:
	RigidBody(double mass_in, Transform& tr_ptr, bool is_st);

	void update(float dt);

	void apply_impulse(const glm::vec3 impulse);

	void apply_epa(glm::vec3 epa);
	
	void apply_rotational_impulse(const glm::vec3& point, const glm::vec3& impulse);

	void add_force(glm::vec3 force_in);

	void add_angular_force(glm::vec3 force_in);

	glm::mat4 create_box_inertia_tensor(float mass, const glm::vec3& half_lengths);

	void reset();

	void set_pos(glm::vec3 pos);

	glm::vec3 get_pos() const;

	glm::mat4 get_quatmat();

	bool& get_is_static_ref();
	
	//TODO(darius) stop fucking with incapsulation
	glm::quat& get_orientation_quaternion_ref();

	glm::quat normalize_quat(const glm::quat& q);

	glm::quat construct_quat(const glm::vec3& v, float theta);

	glm::quat multiply_quats(const glm::quat& u, const glm::quat& v);

	void set_quat_from_angles();

	float& get_ex();

	float& get_ey();

	float& get_ez();

	Transform& tr;

	//euler angles;
	float ex = 0;
	float ey = 0;
	float ez = 0;

	float mass = 1;
	//vec3 position;
	//quat orientation;

	glm::vec3 velocity = { 0,0,0 };
	glm::vec3 angular_velocity = {0,0,0};

	//mat4 inertia_tensor;
	glm::mat4 inertia_tensor = glm::mat4(1.0);
	glm::vec3 force_accumulator = { 0,-1,0 };
	glm::vec3 torque_accumulator = { 0,0,0 };

	float restitution = 0; 
	float friction = 0;

	bool is_static = true;
};
