#include <Scene.h>
#include <Object.h>
#include <Colider.h>
#include <Model.h>
#include <ParticleSystem.h>


Scene::Scene()
{
	init_memory();
	//start_scripts();
}

Object* Scene::createEntity(Object* po, std::string path, Shader sv, LightingShaderRoutine shaderRoutine_in, bool rotateTextures = false) {
	Model m = Model(path, shaderRoutine_in, sv, rotateTextures);
	auto meshes = m.loadModel();

	std::vector<Object*> subobjects;
	subobjects.reserve(meshes.size());
	for (int i = 0; i < meshes.size(); ++i) {
		Object* pt = mem_man.construct(po, meshes[i], sv, shaderRoutine_in);
		std::cout << "created pt\n";
		subobjects.push_back(pt);
	}

	po->set_child_objects(std::move(subobjects));

	po->startScript();
	return po;
}

Object* Scene::AddEmpty(int i)
{
	Object* pt;
	pt = mem_man.construct("empty " + std::to_string(i));
	sceneObjects.push_back(pt);
	return pt;
}

Object* Scene::createSubobject(Object* obj, int i)
{
	Object* pt;
	pt = mem_man.construct("empty " + std::to_string(i));
	obj->addChild(pt);

	return pt;
}

void Scene::destroyObject(size_t id)
{
	auto* ptr = sceneObjects[id];
	mem_man.destroy(ptr);
	sceneObjects[id] = nullptr;
}

void Scene::updateScene() {
	update_objects();
}

void Scene::renderScene()
{
	for (int i = 0; i < sceneObjects.size(); ++i) {
		if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
			continue;
		sceneObjects[i]->renderObject();
	}
}

void Scene::renderParticles()
{
	for (int i = 0; i < sceneObjects.size(); ++i) {
		if (!sceneObjects[i]->getParticleSystem())
			continue;

		sceneObjects[i]->getParticleSystem()->updateUniform3DDistribution(static_cast<float>(glfwGetTime()));
		sceneObjects[i]->getParticleSystem()->renderParticles();
	}
}

void Scene::updateSpriteAnimations(float dt) 
{
	for (int i = 0; i < sceneObjects.size(); ++i) 
	{
		sceneObjects[i]->updateSpriteAnimation(dt);
	}
}

void Scene::updateAnimators(float dt)
{
	for (auto& obj : sceneObjects) {
		if (!obj->getAnimator())
			continue;
		obj->updateAnimator(dt);
	}
}

Object* Scene::get_object_at(int i)
{
	return sceneObjects.at(i);
}

std::vector<Object*>& Scene::get_objects()
{
	return sceneObjects;
}

void Scene::init_memory()
{
	mem_man.allocate(1000);
}

void Scene::start_scripts()
{
	std::cout << "Starting scripts..\n";
	for (auto& i : sceneObjects)
	{
		i->startScript();
	}
}

FlatMesh* Scene::createFlatMesh()
{
	return new FlatMesh();//TODO(darius) new bad
}

void Scene::deleteFlatMesh(FlatMesh* mesh)
{
	delete mesh;
}

void Scene::update_objects() {
	//now its traverse of objects and update. Its much better to do it in one traverse
	//TODO(darius) make it separated threads for collisions and rendering and update?

	for (int i = 0; i < sceneObjects.size(); ++i) {
		if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
			continue;


		sceneObjects[i]->updateScript();


		if (sceneObjects[i]->getColider() && !sceneObjects[i]->getColider()->is_active()) {
			sceneObjects[i]->updatePos();
			continue;
		}

		//COLLISIONS RESOLUTION:
		// O(n^2) 
		// sort by tag + traverse in O(Nlg + N)?
		// make it two types of collision detection: 1) important collision inside renderer thread 2) queued colllision that processed in separate thread?

		bool is_there_collision = false;
		for (int j = 0; j < sceneObjects.size(); ++j) {
			if (
				i == j
				|| !sceneObjects[i]->getColider()
				|| !sceneObjects[j]->getColider()
				|| sceneObjects[i]->getColider()->get_tag() != sceneObjects[j]->getColider()->get_tag()
				|| !sceneObjects[j]->getColider()->is_active()
				)	continue;

			auto collision_state = sceneObjects[i]->getColider()->gjk(&sceneObjects[i]->getColider().value(), &sceneObjects[j]->getColider().value());

			if (collision_state) {
				is_there_collision = collision_state;
				glm::vec3 epa = sceneObjects[i]->getColider()->get_epa();

				if (sceneObjects[i]->getRigidBody() && sceneObjects[j]->getRigidBody() && epa.x == epa.x && epa.y == epa.y && epa.z == epa.z) {
					sceneObjects[i]->getRigidBody()->tr.position += glm::vec3{ epa.x / 4, epa.y / 4, epa.z / 4 };
				}

				break;
			}
		}
		if (!is_there_collision) {
			sceneObjects[i]->updatePos();
		}
	}
}

void Scene::update_scripts()
{
	for (auto& i : sceneObjects)
	{
		i->updateScript();
	}
}

