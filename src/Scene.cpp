#include <Scene.h>
#include <Object.h>
#include <Colider.h>
#include <Model.h>
#include <ParticleSystem.h>
#include <Renderer.h>

#include <string>
#include <iostream>
#include <fstream>


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

Object* Scene::getObjectByName(std::string_view name)
{
	for (auto* objp : sceneObjects) 
	{
		if (objp->get_name() == name)
			return objp;
	}

	return nullptr;
}

Object* Scene::getObjectByID(int ID)
{
	std::cout << "ID SEARCH STARTED\n";
	for (auto* i : sceneObjects) 
	{
		std::cout << i->getID() << "|";
		if (ID == i->getID())
		{
			std::cout << "ID SEARCH ENDED\n";
			return i;
		}
	}
	std::cout << "ID SEARCH ENDED\n";
	return nullptr;
}

std::vector<Object*>& Scene::get_objects()
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
	int id = 1;
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
	return new EmptyScriptRoutine(path, GameState::instance);
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

void Scene::update_objects() {
	//now its traverse of objects and update. Its much better to do it in one traverse
	//TODO(darius) make it separated threads for collisions and rendering and update?
	updateObjectsIDs();

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

			//auto collision_state = sceneObjects[i]->getColider()->gjk(&sceneObjects[i]->getColider().value(), &sceneObjects[j]->getColider().value());
			auto collision_state = sceneObjects[i]->getColider()->check_collision(sceneObjects[j]->getColider().value());

			if (collision_state != glm::vec3{0,0,0}) {
				std::cout << "collision of" << sceneObjects[i]->get_name() << "\n";
				////is_there_collision = true;

				//glm::vec3 epa = sceneObjects[i]->getColider()->get_epa();
				//std::cout << epa.x << " " << epa.y << " " << epa.z << "\n";
				std::cout << collision_state.x << " " << collision_state.y << "\n";
				
				//NOTE(darius) this is a trick to check if float is Nan
				//TODO(darius) not good
				/*if (epa.x == epa.x && epa.y == epa.y && epa.z == epa.z)
				{
					sceneObjects[i]->getTransform().position += glm::vec3{ -0.1, -0.1,0};
					*sceneObjects[i]->getColider()->get_collision_state() = false;
				}
				*/

				sceneObjects[i]->getTransform().position += collision_state;
				*sceneObjects[i]->getColider()->get_collision_state() = false;

				/*
				if (sceneObjects[i]->getRigidBody() && sceneObjects[j]->getRigidBody() && epa.x == epa.x && epa.y == epa.y && epa.z == epa.z) {
					sceneObjects[i]->getRigidBody()->tr.position += glm::vec3{ epa.x / 4, epa.y / 4, epa.z / 4 };
				}
				*/

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

void Scene::serialize(std::string_view path)
{
	std::ofstream file(path.data());
    if(!file.is_open())
        return;
    
	for (auto& i : sceneObjects)
	{
		i->serialize(file);
	}

    file.close();
}

//TODO(darius) check for names being different, and check if object that you created dont exist already
void Scene::deserialize(std::string_view path)
{

	std::ifstream file(path.data());
	if (!file.is_open())
		return;
	
	std::string data;
	std::string line;
	//TODO(darius) optimize it
	while (std::getline(file, line)) 
	{
		data += line;
	}

	std::vector<std::string> objectTokens;

	{
		size_t i = 0;

		while (i < data.size()) 
		{
			size_t oPos1 = data.find("Object", i);
			size_t oPos2 = data.find("Object", oPos1+1);
			i = oPos2;
			objectTokens.push_back(data.substr(oPos1, oPos2-oPos1));
		}
	}

	std::vector<std::string> names;
	std::vector<bool> hiddenStates;

	for (std::string_view tkn : objectTokens) 
	{
		names.push_back(extractNameFromToken(tkn));
		hiddenStates.push_back(extractHiddenStateFromToken(tkn));
	}

	std::vector<Transform>  transs;

	for (std::string_view tkn : objectTokens) 
	{
		transs.push_back(extractTransformFromToken(tkn));
	}

	std::vector<Model> models;
	
	for (std::string_view tkn : objectTokens) 
	{
		models.push_back(extractModelFromToken(tkn));
	}

	std::vector<std::string> scripts;

	for (std::string_view tkn : objectTokens) 
	{
		scripts.push_back(extractScriptFromToken(tkn));
	}

	//Recreation here after
	/*Shader vshdr = Shader(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER);
	Shader fshdr = Shader(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER);
	vshdr.compile();
	fshdr.compile();
	vshdr.link(fshdr);
	*/

	for (int i = 0; i < objectTokens.size(); ++i)
	{
		LightingShaderRoutine shaderRoutine = { Shader(Renderer::shaderLibInstance->getCurrShader())};

		auto* obj = AddObject(std::string(names[i]));
		if (hiddenStates[i])
			obj->hide();
		else
			obj->unhide();

		obj->getTransform() = transs[i];

		if (models.size() > i) 
		{
			obj->getModel() = (models[i]);
			obj->getModel()->setShader(Renderer::shaderLibInstance->getCurrShader());
			obj->getModel()->setShaderRoutine(shaderRoutine);

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
		}

		if(scripts[i] != "None")
			obj->addScript(this, createRoutine(scripts[i]));

		//auto* obj = createObject(std::string(names[i]), transs[i].position, transs[i].scale, glm::vec3{0,0,0}, std::move(models[i]), vshdr, std::move(shaderRoutine), this, nullptr);
	}

	file.close();

	start_scripts();
}

std::string Scene::extractNameFromToken(std::string_view tkn)
{
	size_t nameStart = tkn.find("Name");
	size_t valueStart = nameStart + 9;
	size_t valueEnd = tkn.find("\t", valueStart);
	return std::string(tkn.substr(valueStart, valueEnd - valueStart));
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

	return Transform(pos, scale);
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

			textures.push_back(Texture(TextureFromFile(path.c_str(), false, false), path, "texture_diffuse"));

			break;
		}

		/*
		Shader vshdr = Shader(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER);
		Shader fshdr = Shader(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER);
		vshdr.compile();
		fshdr.compile();
		vshdr.link(fshdr);
		LightingShaderRoutine shaderRoutine = { Shader(vshdr) };*/


		Mesh m(vertices, indices, textures);
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

	size_t i = tkn.find("Point:", animStart);

	if (i == std::string::npos)
	{
		size_t animationPathStart = tkn.find("AnimationPath: {", animStart);
		if(animationPathStart == std::string::npos)
			return std::nullopt;

		size_t pathStart = tkn.find("{", animationPathStart);
		size_t pathEnd = tkn.find("}", pathStart);
		
		SpriteAnimation res = SpriteAnimation(std::string(tkn.substr(pathStart+1, pathEnd - pathStart-1)));

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
	size_t xEnd = line.find(" ");
	std::string xstr(line.substr(xStart + 1, xEnd));
	std::istringstream in(xstr);
	in >> x;

	size_t yStart = xEnd;
	size_t yEnd = line.find(" ", xEnd + 1);
	std::string ystr(line.substr(yStart + 1, yEnd - yStart - 1));
	std::istringstream iny(ystr);
	iny >> y;

	size_t zStart = yEnd;
	size_t zEnd = line.find(" ", yEnd + 1);
	std::string zstr(line.substr(zStart + 1, zEnd - zStart - 1));
	std::istringstream inz(zstr);
	inz >> z;

	return glm::vec3(x, y, z);
}

glm::vec4 Scene::extractVector4FromToken(std::string_view tkn)
{
	size_t brkctStart = tkn.find("{");
	size_t brkctEnd = tkn.find("}");
	std::string line(tkn.substr(brkctStart, brkctEnd - brkctStart));

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;

	size_t xStart = 0;
	size_t xEnd = line.find(" ");
	std::string xstr(line.substr(xStart + 1, xEnd));
	std::istringstream in(xstr);
	in >> x;

	size_t yStart = xEnd;
	size_t yEnd = line.find(" ", xEnd + 1);
	std::string ystr(line.substr(yStart + 1, yEnd - yStart - 1));
	std::istringstream iny(ystr);
	iny >> y;

	size_t zStart = yEnd;
	size_t zEnd = line.find(" ", yEnd + 1);
	std::string zstr(line.substr(zStart + 1, zEnd - zStart - 1));
	std::istringstream inz(zstr);
	inz >> z;

	size_t wStart = zEnd;
	size_t wEnd = line.find(" ", zEnd + 1);
	std::string wstr(line.substr(wStart + 1, wEnd - wStart - 1));
	std::istringstream inw(wstr);
	inw >> w;

	return glm::vec4(x, y, z, w);
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
