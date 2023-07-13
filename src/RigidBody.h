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
	RigidBody(double mass_in, Transform& tr_ptr, bool is_st) : tr(tr_ptr), mass(static_cast<float>(mass_in)), is_static(is_st)
	{
		//startPosition = tr.position;
		tr.q = construct_quat({0,0,0}, 0);
	}

	void update(float dt)
	{
		if (is_static)
			return;
	
		glm::vec3 acceleration = force_accumulator;
		acceleration /= mass;
		velocity = velocity + dt * acceleration;
		velocity *= 0.98;
		tr.position = tr.position + dt * velocity;

		glm::vec4 angular_accelerationtmp = glm::inverse(inertia_tensor) * glm::vec4(torque_accumulator.x, torque_accumulator.y, torque_accumulator.z, 0);
		glm::vec3 angular_acceleration = { angular_accelerationtmp.x, angular_accelerationtmp.y, angular_accelerationtmp.z};
		angular_velocity = angular_velocity + dt * angular_acceleration;
		angular_velocity *= 0.98;

		tr.q = construct_quat(angular_velocity, dt) * tr.q;
	}

	void apply_impulse(const glm::vec3 impulse) {
		if (is_static) {
			return;
		}

		if(glm::length(velocity) < 0.2)
			return;

		velocity = velocity + impulse;
	}

	void apply_epa(glm::vec3 epa)
	{
		if (is_static)
			return;
		apply_impulse({0,1,0});
		//tr.position += epa;
	}
	
	void apply_rotational_impulse(const glm::vec3& point, const glm::vec3& impulse) {
		if (is_static) {
			return;
		}

		glm::vec3 torque = glm::cross((point - tr.position), impulse);
		glm::vec3 angular_acceleration = glm::inverse(inertia_tensor) * glm::vec4(torque.x, torque.y, torque.z, 0);
		angular_velocity = angular_velocity + angular_acceleration;
	}

	void add_force(glm::vec3 force_in)
	{
		if(glm::length(force_in) < 0.2)
			return;
		force_accumulator += glm::vec3(force_in.x, force_in.y, force_in.z);
	}

	void add_angular_force(glm::vec3 force_in)
	{
		angular_velocity = force_in;//vec3{ force_in.x, force_in.y, force_in.z };
	}

	glm::mat4 create_box_inertia_tensor(float mass, const glm::vec3& half_lengths) {
		auto m = glm::mat4(
			(1.0 / 12.0) * mass * (half_lengths.x + half_lengths.y), 0.0, 0.0, 0.0,
			0.0, (1.0 / 12.0) * mass * (half_lengths.y + half_lengths.z), 0.0, 0.0,
			0.0, 0.0, (1.0 / 12.0) * mass * (half_lengths.z + half_lengths.x), 0.0,
			0.0, 0.0, 0.0, 1.0
		);
		inertia_tensor = m;

		return m;
	}

	void reset()
	{
		tr.position = {0,0,0};
		velocity = { 0,0,0 };
	}

	void set_pos(glm::vec3 pos) 
	{
		(tr.position) = pos;
	}

	glm::vec3 get_pos() const 
	{
		return tr.position;
	}

	glm::mat4 get_quatmat()
	{
		glm::mat4 RotationMatrix = glm::toMat4(tr.q);
		return RotationMatrix;
	}

	bool& get_is_static_ref()
	{
		return is_static;
	}
	
	//TODO(darius) stop fucking with incapsulation
	glm::quat& get_orientation_quaternion_ref()
	{
		return tr.q;
	}

	glm::quat normalize_quat(glm::quat q)
	{
		float mag = glm::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);

		if (mag == 0) {
			return q;
		}

		return glm::quat(q.w / mag, q.x / mag, q.y / mag, q.z / mag);
	}

	glm::quat construct_quat(const glm::vec3& v, float theta)
	{
		float temp = sin(theta / 2.0f);
		float x = temp * v.x;
		float y = temp * v.y;
		float z = temp * v.z;
		float w = cos(theta / 2.0f);
		glm::quat qres(w, x, y, z);

		return normalize_quat(qres);
		
		}

	glm::quat multiply_quats(const glm::quat u, const glm::quat v)
	{
		float x = v.w * u.x + v.x * u.w - v.y * u.z + v.z * u.y;
		float y = v.w * u.y + v.x * u.z + v.y * u.w - v.z * u.x;
		float z = v.w * u.z - v.x * u.y + v.y * u.x + v.z * u.w;
		float w = v.w * u.w - v.x * u.x - v.y * u.y - v.z * u.z;

		return normalize_quat(glm::quat(w, x, y, z));
	}

	void set_quat_from_angles()
	{
		if (!is_static)
			return;

		glm::mat4 transformX = glm::eulerAngleX(ex);
		glm::mat4 transformY = glm::eulerAngleY(ey);
		glm::mat4 transformZ = glm::eulerAngleZ(ez);

		glm::mat4 transform1 =
			transformX * transformY * transformZ;

		tr.q = glm::quat_cast(transform1);
	}

	float& get_ex() {
		return ex;
	}

	float& get_ey() {
		return ey;
	}

	float& get_ez() {
		return ez;
	}

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
	glm::vec3 force_accumulator = { 0,0,0 };
	glm::vec3 torque_accumulator = { 0,0,0 };

	float restitution = 0; 
	float friction = 0;

	bool is_static = true;
};
