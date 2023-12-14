#include <Editor.h>
#include <Scene.h>
#include <Object.h>
#include <Colider.h>
#include <Model.h>
#include <ParticleSystem.h>


#include <string>
#include <iostream>
#include <fstream>
#include <ranges>

Scene::Scene()
{
	init_memory();
	//start_scripts();
}

Object* Scene::createEntity(Object* po, std::string path, bool rotateTextures = false) 
{
	Model m = Model(std::move(path));
	auto meshes = m.loadModel();

	std::vector<Object*> subobjects;
	subobjects.reserve(meshes.size());
	for (int i = 0; i < meshes.size()/2; ++i) {
		Object* pt = mem_man.construct("cihld");
		pt->getModel().emplace(meshes[i]);
		std::cout << "created pt\n";
		subobjects.push_back(pt);
	}

	po->setChildObjects(std::move(subobjects));

	po->startScript();

	return po;
}

Object* Scene::AddEmpty(int i)
{
	Object* pt;
	pt = mem_man.construct("empty " + std::to_string(i));
	sceneObjects.push_back(pt);

	emptyIndex = i;

	return pt;
}

Object* Scene::AddObject(const std::string& name) 
{
	Object* pt;
	pt = mem_man.construct(name);
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
	assert(id < sceneObjects.size());
	auto* ptr = sceneObjects[id];
	mem_man.destroy(ptr);
	sceneObjects[id] = nullptr;
	sceneObjects.erase(sceneObjects.begin() + id);
}

void Scene::destroyObject(std::string_view name)
{
	Object* pt = getObjectByName(std::string(name));	
	destroyObject(pt->getID());
}

void Scene::destroyObject(Object* obj)
{
	destroyObject(obj->getID());
}

void Scene::updateScene() 
{
	update_objects();
	update_scripts();
}

// TODO(darius) instancing and batching(?) somewhere here
// maybe try to first submiut all objects to some instancing agent, that will probe all objects and separate them into groups. 
// Then u will traverse through whis groups and render each group?
void Scene::renderScene()
{
	//println("-------------------------------");
	for (int i = 0; i < sceneObjects.size(); ++i) {
		if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
			continue;
		sceneObjects[i]->renderObject();
	}
	//println("-------------------------------");

	for(auto& t : terrains)
	{
		t.draw();
	}
}

void Scene::renderParticles()
{
	for (int i = 0; i < sceneObjects.size(); ++i) {
		if (!sceneObjects[i]->getParticleSystem())
			continue;

		sceneObjects[i]->getParticleSystem()->update(static_cast<float>(glfwGetTime()));
		sceneObjects[i]->getParticleSystem()->renderParticles();
	}
}

void Scene::updateSpriteAnimations(float dt)
{
	for (int i = 0; i < sceneObjects.size(); ++i) 
	{
		sceneObjects[i]->updateSpriteAnimator(dt);
	}
}

void Scene::updateAnimators(float dt)
{
	for (auto& obj : sceneObjects) {
		if (!obj->getSkeletalAnimation())
			continue;

		obj->updateSkeletalAnimation(dt);
		//println("Animator:"); 
        //print(obj->getAnimator().value());
	}
}

Object* Scene::get_object_at(int i)
{
	return sceneObjects.at(i);
}

Object* Scene::getObjectByName(const std::string& name)
{
	for (auto* objp : sceneObjects) 
	{
		if(!objp)
			continue;

		if (std::strcmp(objp->getName().c_str(), name.c_str()) == 0){
			return objp;
		}
	}

	return nullptr;
}

Object* Scene::getObjectByID(int ID)
{
	//std::cout << "ID SEARCH STARTED\n";
	for (auto* i : sceneObjects) 
	{
		std::cout << i->getID() << "|";
		if (ID == i->getID())
		{
			//std::cout << "ID SEARCH ENDED\n";
			return i;
		}
	}
	//std::cout << "ID SEARCH ENDED\n";
	return nullptr;
}

std::vector<Object*>& Scene::getObjects()
{
	return sceneObjects;
}

int Scene::getEmptyIndex()
{
	return emptyIndex;
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

void Scene::updateObjectsIDs()
{
	int id = 0;//Note(darius) was 1
	for (Object* p : sceneObjects) 
	{
		p->setID(id++);
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

EmptyScriptRoutine* Scene::createRoutine(std::string path)
{
	return new EmptyScriptRoutine(std::move(path), GameState::instance);
}

void Scene::deleteRoutine(EmptyScriptRoutine* r)
{
	delete r;
}


Camera* Scene::createCamera()
{
	return new Camera();
}

void Scene::deleteCamera(Camera* cam)
{
	delete cam;
}

void Scene::update_objects() 
{
	//now its traverse of objects and update. Its much better to do it in one traverse
	//TODO(darius) make it separated threads for collisions and rendering and update?
	updateObjectsIDs();
	
	if (CheckColTime.checkTime() >= 0.02) {
		CheckColTime.clearTime();
		for (int i = 0; i < sceneObjects.size(); ++i) {
			if (!sceneObjects[i]) // in case sceneObjects[i] was deleted by index
				continue;

			sceneObjects[i]->updateScript();

			auto rbodyCopyInitTr = (sceneObjects[i]->getTransform());
			sceneObjects[i]->updateRigidBody();

			if (sceneObjects[i]->getColider() && !sceneObjects[i]->getColider()->is_active()) {
				continue;
			}


		//COLLISIONS RESOLUTION:
		// O(n^2) 
		// sort by tag + traverse in O(Nlg + N)?
		// make it two types of collision detection: 1) important collision inside renderer thread 2) queued colllision that processed in separate thread?
			bool is_there_collision = false;
			for (int j = i + 1; j < sceneObjects.size(); ++j) {
				if (
					!sceneObjects[i]->getColider()
					|| !sceneObjects[j]->getColider()
					|| sceneObjects[i]->getColider()->get_tag() != sceneObjects[j]->getColider()->get_tag()
					//|| !sceneObjects[j]->getColider()->is_active()
					)	continue;

				auto collision_state_gjk = sceneObjects[i]->getColider()->gjk(&sceneObjects[i]->getColider().value(), &sceneObjects[j]->getColider().value());

				auto collision_state = sceneObjects[i]->getColider()->check_collision(sceneObjects[j]->getColider().value());

				//println(sceneObjects[i]->get_name());

				if (collision_state != glm::vec3(0, 0, 0)) {
					//std::cout << "collision of" << sceneObjects[i]->get_name() << "\n";
					sceneObjects[i]->getColider()->collider_debug_color = { 1,0,0,0 };
					sceneObjects[j]->getColider()->collider_debug_color = { 1,0,0,0 };
					is_there_collision = true;

					//glm::vec3 epa = sceneObjects[i]->getColider()->get_epa();
					//std::cout << epa.x << " " << epa.y << " " << epa.z << "\n";
					//std::cout << collision_state.x << " " << collision_state.y << "\n";

					//NOTE(darius) this is a trick to check if float is Nan
					//TODO(darius) not good
					/*if (epa.x == epa.x && epa.y == epa.y && epa.z == epa.z)
					{
						sceneObjects[i]->getTransform().position += glm::vec3{ -0.1, -0.1,0};
						*sceneObjects[i]->getColider()->get_collision_state() = false;
					}
					*/

					sceneObjects[i]->getTransform().addToPosition(collision_state);
					*sceneObjects[i]->getColider()->get_collision_state() = false;

					/*
					if (sceneObjects[i]->getRigidBody() && sceneObjects[j]->getRigidBody() && epa.x == epa.x && epa.y == epa.y && epa.z == epa.z) {
						sceneObjects[i]->getRigidBody()->tr.position += glm::vec3{ epa.x / 4, epa.y / 4, epa.z / 4 };
					}
					*/

					break;
				}
				else
				{
					sceneObjects[i]->getColider()->collider_debug_color = { 0,1,0,0 };
					sceneObjects[j]->getColider()->collider_debug_color = { 0,1,0,0 };
				}
			}
			if (is_there_collision) {
				//sceneObjects[i]->updatePos();
				sceneObjects[i]->getTransform() = rbodyCopyInitTr;
				sceneObjects[i]->getRigidBody()->resetFroces();
			}
			else{
				//sceneObjects[i]->getRigidBody()->resetFroces();
			}
		}
	}
}

void Scene::batchProbeSimilarObjects()
{
	for(auto& objI : sceneObjects)
	{
		if(!objI->getModel() || objI->getModel()->meshes.size() < 1 //|| objI->getModel()->meshes[0].getDrawMode() != DrawMode::DRAW_AS_ELEMENTS 
			|| batcher.contains(objI))
			continue;

		Batch combinationBatch;

		std::cout << "Batching object " << objI->getName() << "...\n";

		glm::vec3 originalScale = objI->getTransform().getScale();
		auto meshI = objI->getModel()->meshes[0];

		for(auto& objJ : sceneObjects)
		{
			if(!objJ->getModel() || objJ->getModel()->meshes.size() < 1 || objI == objJ) 
				//|| objJ->getModel()->meshes[0].getDrawMode() != DrawMode::DRAW_AS_ELEMENTS)
				continue;

			auto meshJ = objJ->getModel()->meshes[0];

			if(meshI.getTexturesRef()[0].get_path() == meshJ.getTexturesRef()[0].get_path())
			{
				//NOTE(darius) i think its kinda works now, look into better approach?
				//TODO(darius) add rotation!
				//NOTE(darius) float precision not cool?
				glm::vec3 shift = (objJ->getTransform().getPosition() - objI->getTransform().getPosition());
				//NOTE(darius)IMPORTANT(darius) * 10 cause of render-space scaling, i belive. Brokes on rotating
				glm::vec3 shiftScaled = glm::vec3(shift.x/originalScale.x * 10, shift.y/originalScale.y * 10, shift.z/originalScale.z * 10);

				meshI.addVerticesBath(meshJ, shiftScaled);
			    Mesh batchCopy(meshI.getVertices(), meshI.getIndices(), meshI.getTextures());
			    objI->getModel()->meshes[0].clearMesh();
		    	objI->getModel()->meshes.clear();

			    objI->getModel()->addMesh(batchCopy);

				combinationBatch.addObject(objJ);
			}
		}

		if(combinationBatch.size() > 0)
			combinationBatch.setOrigin(objI);

		batcher.addBatch(std::move(combinationBatch));
	}

	std::vector<Object*> resultObjects;

	for(auto& v : sceneObjects)
	{
		if(!batcher.contains(v))
			resultObjects.push_back(v);
	}

	sceneObjects = std::move(resultObjects);
}

void Scene::updateObjectsLODs()
{
	if(sceneObjects.size() < 1)
		return;

	Object* obj = sceneObjects[0];
	if(!obj->getModel())
		return;

	Mesh& meshI = obj->getModel()->meshes[0];

	auto indices = meshI.generateLOD();
	Mesh batchCopy(meshI.getVertices(), indices, meshI.getTextures());

    obj->getModel()->meshes[0].clearMesh();
	obj->getModel()->meshes.clear();
    obj->getModel()->addMesh(batchCopy);
}

void Scene::recoverBatchedObjects()
{
	auto cache = batcher.takeBack();

	sceneObjects.insert(sceneObjects.end(), cache.begin(), cache.end());

	auto origins = batcher.takeOrigins();

	/*for(auto v = sceneObjects.begin(); v != sceneObjects.end(); v++)
	{
		if(std::find(origins.begin(), origins.end(), *v) != origins.end())
			v = sceneObjects.erase(v);
	}
	*/

	for(auto& i : origins)
	{
		//TODO(darius) cringe
		if(i->getModel()->meshes.size() > 0)
			i->getModel()->meshes[0].clearBatch(4,6);
		//destroyObject(i);
	}

	batcher.clear();
}

bool Scene::recoverBatch(Object* origin)
{
	if(!batcher.containsOrigin(origin))
		return true;

	auto cache = batcher.takeBack();
	sceneObjects.insert(sceneObjects.end(), cache.begin(), cache.end());

	//auto origins = batcher.takeOrigins();

	/*for(auto v = sceneObjects.begin(); v != sceneObjects.end(); v++)
	{
		if(std::find(origins.begin(), origins.end(), *v) != origins.end())
			v = sceneObjects.erase(v);
	}
	*/

	origin->getModel()->meshes[0].clearBatch(4,6);

	batcher.clear();

	return false;
}

BatchingCache& Scene::getBatchCache()
{
	return batcher;	
}

void Scene::addObjectToNetSync(Object* obj)
{
	networkSync.pushObject(obj);	
}

NetworkSynchronizer& Scene::getNetworkSynchronizer()
{
	return networkSync;
}

void Scene::update_scripts()
{
	for (auto& i : sceneObjects)
	{
		i->updateScript();
	}
}

bool Scene::extractBoolFromToken(std::string_view tkn)
{
	if (tkn.find("true") != std::string::npos)
		return true;
	return false;
}

void Scene::addCameraToScene(Camera* cam)
{
	sceneCameras.push_back(cam);	
}

std::vector<Camera*>& Scene::getSceneCameras()
{
	return sceneCameras;
}

Camera* Scene::getCameraAt(int id)
{
	if(sceneCameras.size() <= id)
		return nullptr;

	return sceneCameras[id];
}

void Scene::addTerrain(Terrain&& trin)
{
	terrains.emplace_back(std::move(trin));
}

void Scene::serialize(std::string_view path)
{
	std::ofstream file(path.data());
    if(!file.is_open())
        return;

    file << "CameraPos: {" << GameState::instance->cam.getCameraPos().x << " " << GameState::instance->cam.getCameraPos().y << " " << GameState::instance->cam.getCameraPos().z << "}\n";
    
	for (auto& i : sceneObjects)
	{
		i->serialize(file);
	}

    file.close();
}

void Scene::serializePrefab(Object* obj, std::string_view path)
{
	std::ofstream file(path.data());

    if(!file.is_open())
    	return;

    obj->serializeAsPrefab(file);

    file.close();
}

std::string Scene::readFileToString(std::string_view path)
{
	std::ifstream file(path.data());
	if (!file.is_open())
		return {};


	std::string data;
	std::string line;
	//TODO(darius) optimize it
	while (std::getline(file, line)) 
	{
		data += line;
	}

	file.close();

	return data;
}

//TODO(darius) check for names being different, and check if object that you created dont exist already
void Scene::deserialize(std::string_view path)
{
	std::string data = readFileToString(path);
	parseScene(data);
}

void Scene::parseSynchronizationMsg(std::string data)
{
	//std::cout << "MSG:\n";
	//std::cout << data;
	std::vector<std::string> objectTokens;

	{
		size_t i = 0;

		while (i < data.size()) 
		{
			size_t oPos1 = data.find("Object", i);
			size_t oPos2 = data.find("Object", oPos1+1);
			//i = data.size();
			//if(oPos2 != std::string::npos)
			i = oPos2;
			objectTokens.emplace_back(data.substr(oPos1, oPos2-oPos1));
		}
	}

	std::cout << "objTkns size: " << objectTokens.size() << "\n";

	std::vector<std::string> names;

	for (std::string_view tkn : objectTokens) 
	{
		names.emplace_back(extractNameFromToken(tkn));
	}

	std::cout << "names:\n";
	for(auto& i : names)
		std::cout << i << "\n";

	std::vector<Transform>  transs;

	for (std::string_view tkn : objectTokens) 
	{
		transs.emplace_back(extractTransformFromToken(tkn));
	}

	//NOTE(darius) synchronization here

	for(int i = 0; i < sceneObjects.size(); ++i)
	{
		for(int j = 0; j < names.size(); ++j)
		{   
			//NOTE(darius) copy other components here?
			if(sceneObjects[i]->getName() == names[j])
				sceneObjects[i]->getTransform() = (transs[j]);
		}
	}
}

//TODO(darius) refactor
//TODO(darius) its not fast nor cool to load stuff and create it at same time. You want to load all the stuff, and start creating stuff after
void Scene::parseScene(std::string_view data)
{
	std::unique_lock<std::mutex> lck(sceneLock);

	size_t cameraPos = data.find("CameraPos: {");

	if (cameraPos != std::string::npos) {
		size_t brcktStart = data.find("CameraPos: {", cameraPos);
		size_t brcktEnd = data.find('}', brcktStart);
		glm::vec3 cameraPosition = extractVectorFromToken(data.substr(brcktStart, brcktEnd - brcktStart));

		GameState::instance->cam.setCameraPos(cameraPosition);
	}

	std::vector<std::string> objectTokens;

	{
		size_t i = 0;

		while (i < data.size()) 
		{
			size_t oPos1 = data.find("Object", i);
			size_t oPos2 = data.find("Object", oPos1+1);
			i = oPos2;
			objectTokens.emplace_back(data.substr(oPos1, oPos2-oPos1));
		}
	}

	std::vector<std::string> names;
	std::vector<bool> hiddenStates;

	//TODO(darius) TEMPO, test it
    //workersUsed.Submit([](){std::cout << "ima hui\n"; });
    //auto res_future = tp.Submit([](int a, int b){ return a - b; }, as, bs);
    //std::cout << res_future.get();

	//workersUsed.Submit([this, &objectTokens, &names, &hiddenStates](){
		for (std::string_view tkn : objectTokens) 
		{
			names.emplace_back(extractNameFromToken(tkn));
			hiddenStates.emplace_back(extractHiddenStateFromToken(tkn));
		}
	//}
	//);

	std::vector<Transform>  transs;

	for (std::string_view tkn : objectTokens) 
	{
		transs.emplace_back(extractTransformFromToken(tkn));
	}

	std::vector<Model> models;
	
	for (std::string_view tkn : objectTokens) 
	{
		models.emplace_back(extractModelFromToken(tkn));
	}

	std::vector<std::string> scripts;

	for (std::string_view tkn : objectTokens) 
	{
		scripts.emplace_back(extractScriptFromToken(tkn));
	}

	for (int i = 0; i < objectTokens.size(); ++i)
	{
		auto* obj = AddObject(std::string(names[i]));
		if (hiddenStates[i])
			obj->hide();
		else
			obj->unhide();

		obj->getTransform() = transs[i];

		if (models.size() > i) 
		{
			obj->getModel() = (models[i]);

			auto anim = extractSpriteAnimationFromToken(objectTokens[i]);
			if(anim)
				obj->addSpriteAnimation(std::move(*anim));

			auto col = extractColliderFromToken(objectTokens[i]);
			if (col)
				obj->addCollider(col->get_size(), col -> get_shift());
			
			auto body = extractRigidBodyFromToken(objectTokens[i]);
			if (body)
				obj->addRigidBody(body->mass);

			auto pointLight = extractPointLightFromToken(objectTokens[i]);
			if (pointLight) {
				glm::vec3 pos = pointLight->position;
				obj->addPointLight(std::move(*pointLight), pos);
			}

			auto particles = extractParticleSystemFromToken(objectTokens[i]);	
			if(particles){
				obj->addParticleSystem(std::move(*particles));
			}
		}

		if(scripts[i] != "None")
			obj->addScript(this, createRoutine(scripts[i]));

		//auto* obj = createObject(std::string(names[i]), transs[i].position, transs[i].scale, glm::vec3{0,0,0}, std::move(models[i]), vshdr, std::move(shaderRoutine), this, nullptr);
	}

	start_scripts();
}

std::string Scene::extractNameFromToken(std::string_view tkn)
{
	size_t nameStart = tkn.find("Name");
	size_t valueStart = tkn.find("{", nameStart);//nameStart + 9;
	size_t valueEnd = tkn.find("}", valueStart);

	auto str = std::string(tkn.substr(valueStart + 3, valueEnd - valueStart - 4));

	str.erase(std::remove_if(str.begin(), str.end(),
		[](char c) { return !(std::isalpha(c) || std::isdigit(c)); }),
		str.end());

	return str;
}

bool Scene::extractHiddenStateFromToken(std::string_view tkn)
{
	size_t stateStart = tkn.find("Hidden:");
	if(stateStart == std::string::npos)
		return false;
	size_t stateEnd = tkn.find("}", stateStart);
	auto st = tkn.substr(stateStart, stateEnd - stateStart);
	return extractBoolFromToken(st);
}

Transform Scene::extractTransformFromToken(std::string_view tkn)
{
	size_t transStart = tkn.find("Transform");

	size_t posStart = tkn.find("Position", transStart);
	glm::vec3 pos = extractVectorFromToken(tkn.substr(posStart));

	size_t scaleStart = tkn.find("Scale", transStart);
	glm::vec3 scale = extractVectorFromToken(tkn.substr(scaleStart));

	size_t quatStart = tkn.find("Quaternion", scaleStart);
	glm::vec4 quatValues = extractVector4FromToken(tkn.substr(quatStart));

	glm::quat rot = glm::quat(quatValues.w, quatValues.x, quatValues.y, quatValues.z);

	size_t matStart = tkn.find("Matrix: ", transStart);

	size_t brcktStart = tkn.find("{", matStart);
	size_t matEnd = tkn.find("}", matStart);

	std::vector<float> matrix;

	size_t parsePos = brcktStart;
	size_t spacePos = parsePos;

	for(int i = 0; i < 16; ++i)
	{
		std::string xstr(tkn.substr(spacePos + 1, matEnd - spacePos - 1));

		spacePos = tkn.find(" ", spacePos+1);

		std::istringstream in(xstr);

		float x = 0.0f;
		in >> x;

		matrix.push_back(x);
	}

	//auto res = Transform(pos, scale, rot);
	//res.rotate(rot);

	auto res = Transform(matrix);

	return res;
}

Model Scene::extractModelFromToken(std::string_view tkn)
{
	size_t pathStart = tkn.find("Path");
	if(pathStart == std::string::npos)
		return extractMeshesFromToken(tkn);

	size_t pathEnd = tkn.find("}", pathStart);
	
	std::string path(tkn.substr(pathStart + 7, pathEnd - pathStart - 7));
	if (path != "") {
		/*Shader vshdr = Shader(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER);
		Shader fshdr = Shader(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER);
		vshdr.compile();
		fshdr.compile();
		vshdr.link(fshdr);
		LightingShaderRoutine shaderRoutine = { Shader(vshdr) };
		*/
		return Model(path);
	}
	return extractMeshesFromToken(tkn);
}

Model Scene::extractMeshesFromToken(std::string_view tkn) 
{	
	Model res;
	size_t i = 0;
	while (i < tkn.size()) {
		size_t verticesStart = tkn.find("Vertices", i+1);
		i = verticesStart;
		if (verticesStart == std::string::npos)
			break;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;

		//TODO(darius) why i???
		size_t i = verticesStart;

		while (i < tkn.size())
		{
			//DANGER(darius)NOTE(darius)TODO(darius) this will look for "Position" until the end of token, 
			//so it will loop on other Position variables of other objects
			size_t pPos = tkn.find("Position", i);
			if (pPos == std::string::npos)
				break;
			size_t nPos = tkn.find("Normal", pPos + 1);

			positions.push_back(extractVectorFromToken(tkn.substr(pPos + 1, nPos - pPos)));

			size_t tcPos = tkn.find("TexCoords", nPos + 1);
			normals.push_back(extractVectorFromToken(tkn.substr(nPos + 1, tcPos - nPos)));

			size_t pPos2 = tkn.find("Position", tcPos + 1);
			texcoords.push_back(extractVectorFromToken(tkn.substr(tcPos + 1, pPos2 - tcPos)));

			i = tcPos;
		}

		//NOTE(darius) separated step for testing purpses
		std::vector<Vertex> vertices;

		for (int i = 0; i < positions.size(); ++i)
		{
			vertices.push_back(Vertex{ positions[i], normals[i], texcoords[i] });
			//std::cout << vertices[i].TexCoords.x << " " << vertices[i].TexCoords.y << "\n"; //<< vertices[i].Normal.z << "\n";
		}

		std::vector<unsigned int> indices;

		size_t indicesStart = tkn.find("Indices:");
		i = indicesStart;

		while (i < tkn.size())
		{
			size_t indPos = tkn.find("Indice:", i);
			if (indPos == std::string::npos)
				break;

			size_t brckStart = tkn.find("{", indPos);
			size_t brckEnd = tkn.find("}", brckStart);
			std::string ind(tkn.substr(brckStart + 1, brckEnd - brckStart - 1));
			//TODO(darius) cringe
			indices.push_back(std::stoi(ind));
			i = brckEnd;
		}

		std::vector<Texture> textures;
		size_t texturesStart = tkn.find("Textures:");
		i = texturesStart;

		while (i < tkn.size())
		{
			size_t texPos = tkn.find("Texture:", i);
			if (texPos == std::string::npos)
				break;

			size_t brckStart = tkn.find("{", texPos);
			size_t brckEnd = tkn.find("}", brckStart);

			std::string path(tkn.substr(brckStart + 1, brckEnd - brckStart - 1));

			size_t texTypePos = tkn.find("Type:", i);

			std::string textureType;

			if (texPos == std::string::npos)
			{
				break;
				textureType = "texture_diffuse";
			}

			brckStart = tkn.find("{", texTypePos);
			brckEnd = tkn.find("}", brckStart);

			textureType = (tkn.substr(brckStart + 1, brckEnd - brckStart - 1));

			textures.emplace_back(Texture(ImageUtils::TextureFromFile(path.c_str(), true), path, textureType));

			i = brckEnd;

			//break;
		}

		/*
		Shader vshdr = Shader(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER);
		Shader fshdr = Shader(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER);
		vshdr.compile();
		fshdr.compile();
		vshdr.link(fshdr);
		LightingShaderRoutine shaderRoutine = { Shader(vshdr) };*/

		size_t drawModeStart = tkn.find("DrawMode:");
		size_t brckStart = tkn.find("{", drawModeStart);
		size_t brckEnd = tkn.find("}", brckStart);
		std::string modeStr(tkn.substr(brckStart + 1, brckEnd - brckStart - 1));

		unsigned int mode = std::stoi(modeStr);

		Mesh m(vertices, indices, textures);
		m.setDrawMode((DrawMode)mode);

		res.addMesh(m);
	}

	return res;
	//return Model(m, vshdr, shaderRoutine);
}

std::optional<Colider> Scene::extractColliderFromToken(std::string_view tkn)
{
	size_t colStart = tkn.find("Collider:");

	if (colStart == std::string::npos)
		return std::nullopt;
	
	size_t sizeStart = tkn.find("Size:", colStart);
	size_t shiftStart = tkn.find("Shift:", colStart);

	glm::vec3 size = extractVectorFromToken(tkn.substr(sizeStart, shiftStart));
	
	size_t shiftEnd = tkn.find("Model", shiftStart);

	glm::vec3 shift = extractVectorFromToken(tkn.substr(shiftStart, shiftEnd));
	//std::cout << shift.x << "|" << shift.y << "|" << shift.z << "\n";

	Transform tr;

	Colider res(size, tr);
	res.set_shift(shift);

	return res;
}

std::optional<RigidBody> Scene::extractRigidBodyFromToken(std::string_view tkn)
{
	size_t bodyStart = tkn.find("RigidBody:");

	if (bodyStart == std::string::npos)
		return std::nullopt;
	
	size_t brcktStart = tkn.find("{", bodyStart);
	size_t brcktEnd = tkn.find("}", brcktStart);
	glm::vec3 vals = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));

	Transform tr;
	RigidBody res(vals.x, tr, vals.y);

	return res;
}

std::optional<SpriteAnimation> Scene::extractSpriteAnimationFromToken(std::string_view tkn)
{
	size_t animStart = tkn.find("SpriteAnimation:");
	
	if (animStart == std::string::npos)
		return std::nullopt;

	size_t delayStart = tkn.find("Delay: {", animStart);

	size_t brcktStart = tkn.find("{", delayStart);
	size_t brcktEnd = tkn.find("}", brcktStart);

	float delayVal = std::stoi(std::string(tkn.substr(brcktStart + 1, brcktEnd - brcktStart - 1)));

	size_t i = tkn.find("Point:", animStart);

	if (i == std::string::npos)
	{
		size_t animationPathStart = tkn.find("AnimationPath: {", animStart);
		if(animationPathStart == std::string::npos)
			return std::nullopt;

		size_t pathStart = tkn.find("{", animationPathStart);
		size_t pathEnd = tkn.find("}", pathStart);
		
		//TODO(darius) remove copcupast here and there (747)
		SpriteAnimation res = SpriteAnimation(std::string(tkn.substr(pathStart+1, pathEnd - pathStart-1)));
		*res.getDelay() = delayVal;

		return res;
	}

	std::vector<glm::vec4> points;
	while (i < tkn.size()) 
	{
		size_t vecEnd = tkn.find("}", i);
		points.push_back(extractVector4FromToken(tkn.substr(i, vecEnd - i)));
		i = tkn.find("Point:", vecEnd);
	}

	SpriteAnimation res = SpriteAnimation(points, 100);
	*res.getLength() = static_cast<float>(points.size());
	*res.getDelay() = delayVal;
	return res;
}

std::string Scene::extractScriptFromToken(std::string_view tkn)
{
	size_t scriptStart = tkn.find("Script:");
	if (scriptStart == std::string::npos)
		return "None";

	size_t routineStart = tkn.find("Routine:", scriptStart);

	size_t brcktStart = tkn.find("{", routineStart);
	size_t brcktEnd = tkn.find("}", brcktStart);
	std::string res(tkn.substr(brcktStart + 1 , brcktEnd - brcktStart - 1));

	return res;
}

std::optional<PointLight> Scene::extractPointLightFromToken(std::string_view tkn)
{
	size_t lightStart = tkn.find("PointLight:");

	if (lightStart == std::string::npos)
		return std::nullopt;

	size_t brcktStart = tkn.find("Pos: {", lightStart);
	size_t brcktEnd = tkn.find("}", lightStart);
	glm::vec3 position  = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));

	brcktStart = tkn.find("{", brcktEnd);
	brcktEnd = tkn.find("}", brcktStart);
	glm::vec3 color = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));

	PointLight res;
	res.position = position;
	res.color = color;

	return res;
}

std::optional<ParticleSystem> Scene::extractParticleSystemFromToken(std::string_view tkn)
{
	size_t particlesStart = tkn.find("Particles:");
	if(particlesStart == std::string::npos)
		return std::nullopt;
	size_t meshStart = tkn.find("Mesh:", particlesStart);
	size_t pathStart = tkn.find("Path:", meshStart);

	size_t brcktStart = tkn.find("{", pathStart);
	size_t brcktEnd = tkn.find("}", brcktStart);
	std::string path(tkn.substr(brcktStart + 1 , brcktEnd - brcktStart - 1));

	size_t emitterStart = tkn.find("Emitter:", brcktEnd);
	brcktStart = tkn.find("{", emitterStart);
	brcktEnd = tkn.find("}", brcktStart);
	glm::vec3 emitterVec = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));

	size_t sizeStart = tkn.find("ParticleSize:", brcktEnd);
	brcktStart = tkn.find("{", sizeStart);
	brcktEnd = tkn.find("}", brcktStart);
	glm::vec3 particleSize = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));

	size_t boundStart = tkn.find("Boundaries:", brcktEnd);
	brcktStart = tkn.find("{", boundStart);
	brcktEnd = tkn.find("}", brcktStart);
	glm::vec3 bounds = extractVectorFromToken(tkn.substr(brcktStart, brcktEnd - brcktStart));


	size_t amountStart = tkn.find("ParticlesAmount:", brcktEnd);
	brcktStart = tkn.find("{", amountStart);
	brcktEnd = tkn.find("}", brcktStart);

	int amount = std::stoi(std::string(tkn.substr(brcktStart + 1, brcktEnd - brcktStart - 1)));

	FlatMesh m;
	m.setTexture(path);

	ParticleSystem p(amount);
	p.addParticle(std::move(m));
	p.emitter = emitterVec;
	p.particle_size = particleSize;
	p.minBound = bounds.x;
	p.maxBound = bounds.y;

	return p;
}

//TODO(darius) optimise, test and refactor this shit, or switch to simdjson
//NOTE(darius) weird float magic when load from file result in wrong precision
glm::vec3 Scene::extractVectorFromToken(std::string_view tkn)
{
	size_t brkctStart = tkn.find("{");
	size_t brkctEnd = tkn.find("}");
	std::string line(tkn.substr(brkctStart, brkctEnd - brkctStart));

	float x = 0;
	float y = 0;
	float z = 0;

	size_t xStart = 0;
	size_t xEnd = line.find(' ');
	std::string xstr(line.substr(xStart + 1, xEnd));
	std::istringstream in(xstr);
	in >> x;

	size_t yStart = xEnd;
	size_t yEnd = line.find(' ', xEnd + 1);
	std::string ystr(line.substr(yStart + 1, yEnd - yStart - 1));
	std::istringstream iny(ystr);
	iny >> y;

	size_t zStart = yEnd;
	size_t zEnd = line.find(' ', yEnd + 1);
	std::string zstr(line.substr(zStart + 1, zEnd - zStart - 1));
	std::istringstream inz(zstr);
	inz >> z;

	return glm::vec3(x, y, z);
}

glm::vec4 Scene::extractVector4FromToken(std::string_view tkn)
{
	size_t brkctStart = tkn.find('{');
	size_t brkctEnd = tkn.find('}');
	std::string line(tkn.substr(brkctStart, brkctEnd - brkctStart));

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

	size_t xStart = 0;
	size_t xEnd = line.find(' ');
	std::string xstr(line.substr(xStart + 1, xEnd));
	std::istringstream in(xstr);
	in >> x;

	size_t yStart = xEnd;
	size_t yEnd = line.find(' ', xEnd + 1);
	std::string ystr(line.substr(yStart + 1, yEnd - yStart - 1));
	std::istringstream iny(ystr);
	iny >> y;

	size_t zStart = yEnd;
	size_t zEnd = line.find(' ', yEnd + 1);
	std::string zstr(line.substr(zStart + 1, zEnd - zStart - 1));
	std::istringstream inz(zstr);
	inz >> z;

	size_t wStart = zEnd;
	size_t wEnd = line.find(' ', zEnd + 1);
	std::string wstr(line.substr(wStart + 1, wEnd - wStart - 1));
	std::istringstream inw(wstr);
	inw >> w;

	return glm::vec4(x, y, z, w);
}

