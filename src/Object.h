#pragma once
#include <vector>
#include <optional>
#include <string_view>
#include <functional>
#include <queue>

#include <glm/glm.hpp>

#include <Model.h>
#include <Colider.h>
#include <RigidBody.h>
#include <Transform.h>
#include <Script.h>
#include <Scene.h>

class Colider;

class Object
{
public:
	Object(std::string name_in, glm::vec3 pos_in, glm::vec3 scale_in, std::string_view model_path_in, Shader model_shader, std::function<void(Transform) > shaderRoutine_in,
																									Scene* scn, std::function<void(void*, void*)>&& st, std::function<void(void*, void*)>&& upd, 
																									bool gammaShader = false, bool rotateTextures = false, float angle = 90)
	{
		model = Model(model_path_in, model_shader, shaderRoutine_in, gammaShader, rotateTextures);
		script = Script(scn, this, std::move(st), std::move(upd));
		tr = Transform(pos_in, scale_in);
		
		rbody.emplace(RigidBody(0.1, tr, false));
		rbody.value().create_box_inertia_tensor(10, { 1,1,1 });

		name = name_in;

		colider.emplace(Colider(tr));
	}

	void setupScript(std::function<void(void*, void*)>&& st, std::function<void(void*, void*)>&& upd)
	{
		script.value().set_scripts(std::move(st), std::move(upd));
	}

	void startScript()
	{
		script.value().startScript();
	}

	void updateScript() 
	{
		script.value().updateScript();
	}

	void renderObject() 
	{
		//std::cout << "position of " << name << " = " << rbody.value().get_pos().x << "|" << rbody.value().get_pos().y << "|" << rbody.value().get_pos().z << "\n";
		//std::cout << "tr" << tr.position.x << "\n";
		//std::cout << "for " << name << " " << "address of tr " << &tr << "|" << "address of rbody tr" << &rbody.value().tr << "\n";
		if(!object_hidden)
			model.value().Draw(tr);
	}

	void apply_force(glm::vec3 force) 
	{
		rbody.value().add_force(force);
	}

	void updatePos() 
	{
		//std::cout << name << " force" << rbody.value().force.x << "\n";
		rbody.value().update(1);

		/*if (rbody.value().get_pos().x > 20) {
			rbody.value().reset();
			//rbody.value().add_force({ 0,-1,0 });
			//rbody.value().apply_impulse({ 0,0,1 });
		}
		*/
	}

	glm::vec3 get_pos() const
	{
		return tr.position;
	}
	
	//TODO(darius) fuck you, incapsulation
	glm::vec3& get_pos_ref()
	{
		return tr.position;
	}

	const Colider const& getColider() const
	{
		return colider.value();
	}

	RigidBody& getRigidBody()
	{
		return rbody.value();
	}

	std::vector<Object*>& get_child_objects() 
	{
		return child_opbjects;
	}

	void traverseObjects(std::function<void(Object*)> functor)
	{
		std::queue<Object*> q;
		for (auto& i : child_opbjects)
			q.push(i);
		while(!q.empty())
		{
			auto curr = q.front();
			q.pop();

			functor(curr);

			for (auto& i : curr -> get_child_objects()) {
				q.push(i);
			}
		}
	}

	std::string& get_name() 
	{
		return name;
	}

	void hide()
	{
		object_hidden = true;
	}

	void unhide()
	{
		object_hidden = false;
	}

	bool& object_hidden_state()
	{
		return object_hidden;
	}

private:
	std::optional<Model> model;
	std::optional<RigidBody> rbody;
	std::optional<Colider> colider;
	std::optional<Script> script;
	Transform tr;
	std::string name;
	std::vector<Object*> child_opbjects = {};

	bool object_hidden = false;
};

