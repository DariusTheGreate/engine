#include "Renderer.h"
#include <Colider.h>

DebugRenderer::DebugRenderer() : dsv(GameState::engine_path + "shaders/debugVertexShader.glsl", GL_VERTEX_SHADER), dsf(GameState::engine_path + "shaders/debugFragmentShader.glsl", GL_FRAGMENT_SHADER)
{
	dsv.compile();
	dsf.compile();
	dsv.link(dsf);

	vbo.init();
	ebo.init();
	vao.init();

	vao.bind();

	vbo.bind(48 * 6 * sizeof(float), vertices);
	vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	vbo.vboEnableVertexAttribArray(0);

	ebo.bind(6 * sizeof(unsigned int), indices);

	setupSceneGrid();
}

void DebugRenderer::setupSceneGrid()
{
    grid_mode = GameState::editor_mode;
    std::cout << "grid setup " << grid_mode << "\n";
    indices_grid.clear();
    vertices_grid.clear();
    vertices_grid.shrink_to_fit();
    indices_grid.shrink_to_fit();

	for (int i = 0; i <= slices; ++i) {
		for (int j = 0; j <= slices; ++j) {
			float x = (float)j;
            float y = 0;
            float z = (float)i;

		    if(grid_mode == 2){	
                x = (float)j;
                y = (float)i;
                z = 0;
            }

            if(grid_mode == 3)
                vertices_grid.push_back(glm::vec3((x-slices/2) * grid_scale, (y) * grid_scale, (z-slices/2) * grid_scale));

            else if(grid_mode == 2)
                vertices_grid.push_back(glm::vec3((x-slices/2) * grid_scale, (y-slices/2) * grid_scale, (z) * grid_scale));
		}
	}

	for (int i = 0; i < slices; ++i) {
		for (int j = 0; j < slices; ++j) {
			int row1 = j * (static_cast<int>(slices) + 1);
			int row2 = (i + 1) * (static_cast<int>(slices) + 1);

			indices_grid.push_back(glm::vec4(row1 + j, row1 + j + 1, row1 + j + 1, row2 + j + 1));
			indices_grid.push_back(glm::vec4(row2 + j + 1, row2 + j, row2 + j, row1 + j));
		}
	}

	vao_grid.init();
	VBO vbo_grid;
	vbo_grid.init();
	EBO ebo_grid;
	ebo_grid.init();
	
	vao_grid.bind();
	vbo_grid.bind(vertices_grid.size() * sizeof(glm::vec3), &(vertices_grid[0]));
	vbo_grid.setVAO(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	vbo_grid.vboEnableVertexAttribArray(0);
	ebo_grid.bind(indices_grid.size() * sizeof(glm::uvec4), &(indices_grid[0]));

	length = (GLuint)indices_grid.size() * 4;
	glLineWidth(2);
}

void DebugRenderer::renderDebugColider(Window* wind, std::optional<Colider>& collider, std::optional<RigidBody>& body)
{
	if(!collider)
		return;
	glUseProgram(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, collider->get_transform().position + glm::vec3{collider->get_size().x/2, collider->get_size().y/2, collider->get_size().z/2} - collider->get_render_shift());
	if(body)
		model *= body->get_quatmat();
	//TODO(darius) its not size, its scale
	model = glm::scale(model, collider->get_size());
	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", {0,1,0,0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_LINE_STRIP, 0, 36);
	glBindVertexArray(0);
}

void DebugRenderer::renderDebugCube(glm::vec3 pos, int r)
{
	glUseProgram(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	//TODO(darius) its not size, its scale
	model = glm::scale(model, {r,r,r});
	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", {0,1,0,0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_LINE_STRIP, 0, 36);
	glBindVertexArray(0); 
}

//TODO(darius) make it instanced. Dont upload each point individually
void DebugRenderer::renderDebugPoint(glm::vec3 a, glm::vec4 color = glm::vec4(0,1,0,0))
{
    if(!debug_render_points)
        return;
	glUseProgram(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, a);
	model = glm::scale(model, {0.05,0.05,0.05});
	//TODO(darius) its not size, its scale
	//model = glm::scale(model, {r,r,r});
	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", color);
	//dsv.setMat4("model", model);
	flat.DrawRaw(dsv, 228, a, {0.01,0.01,0.01});
	//vao.bind();
	//glDrawArrays(GL_LINE_STRIP, 0, 36);
	//glBindVertexArray(0);
}

void DebugRenderer::renderDebugGrid()
{
    if(GameState::editor_mode != grid_mode)
    {
        setupSceneGrid(); 
    }

	glUseProgram(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	dsv.setVec4("objectColor", {0.4,0.4,0.4,0});
	dsv.setMat4("model", model);
	vao_grid.bind();
	glDrawElements(GL_LINES, static_cast<int>(length), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void DebugRenderer::renderDebugLightSource(std::optional<PointLight>& p)
{
	if(!p)
		return;
	glUseProgram(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, p->position);
	model = glm::scale(model, glm::vec3{0.2,0.2,0.2});

	dsv.setVec4("objectColor", {p->color.x, p->color.y, p->color.z, 0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void DebugRenderer::updateCamera(glm::mat4 projection, glm::mat4 view)
{
	glUseProgram(dsv.getProgram());
	dsv.setMat4("projection", projection);
	dsv.setMat4("view", view);
}

void DebugRenderer::renderPoints()
{
	for(auto& i : pointsToRender)
		renderDebugPoint(i.point, i.color);
}

void DebugRenderer::clearPoints()
{
	pointsToRender.clear();
	pointsToRender.shrink_to_fit();
}


Renderer::Renderer(Scene* currScene_in, GameState* instance) : currScene(currScene_in), sv(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER),
sf(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER) {
	//TODO(darius) throws exception?
	//routine = EmptyScriptRoutine(GameState::engine_path + "logicScripts/EngineLogic/x64/Debug", instance);

    //pointLight = PointLight(glm::vec3{ -0.2f, -1.0f, -0.3f }, glm::vec3(1, 1, 1));
    //pointLight.addLight();
    //directionalLight = DirectionalLight(glm::vec3{ -0.2f, -1.0f, -0.3f }, glm::vec3(1, 1, 1));
    //directionalLight.ambient = { 0,0,0 };
    //spotLight = SpotLight(glm::vec3{ -0.2f, -1.0f, -0.3f }, glm::vec3(0, -1, 0));

    //glfwSetCursorPos(wind->getWindow(), wind->getWidth() / 2, wind->getHeight() / 2);

    sv.compile();
    sf.compile();
    sv.link(sf);

	currShaderRoutine = {Shader(sv)};

	/*FlatMesh flat;
	FlatMesh idle;
	FlatMesh runAnimTexture;
	FlatMesh runLeftAnimTexture;
	FlatMesh danceAnimation;
	FlatMesh enemyAnimation;

	flat.setTexture(GameState::engine_path + "textures/hornet", "iddle.png");
	idle.setTexture(GameState::engine_path + "textures/hornet", "iddle.png");
	enemyAnimation.setTexture(GameState::engine_path + "textures", "HollowSpid.png");

	runLeftAnimTexture.setTexture(GameState::engine_path + "textures/hornet", "runLeft.png");
	runAnimTexture.setTexture(GameState::engine_path + "textures/hornet", "runRight.png");
	danceAnimation.setTexture(GameState::engine_path + "textures/hornet", "jump.png");

	auto* obj0 = currScene->AddEmpty(228);
	auto* obj1 = currScene->AddEmpty(229);
	auto* player= currScene->AddEmpty(230);
	auto* obj3 = currScene->AddEmpty(231);
	auto* obj4 = currScene->AddEmpty(232);
	auto* enemy = currScene->AddEmpty(232);

	obj0->hide();
	obj1->hide();
	obj3->hide();
	obj4->hide();

	obj0 ->addModel(std::move(flat), sv, currShaderRoutine);
	obj1->addModel(std::move(runAnimTexture), sv, currShaderRoutine);
	obj3->addModel(std::move(runLeftAnimTexture), sv, currShaderRoutine);
	obj4->addModel(std::move(danceAnimation), sv, currShaderRoutine);
	player->addModel(std::move(idle), sv, currShaderRoutine);
	enemy->addModel(std::move(enemyAnimation), sv, currShaderRoutine);

	auto enemySprite = SpriteAnimation(4, 8, 100);
	enemy->setSpriteAnimation(enemySprite);
	enemy->moveTransform({5,0,0});
	enemy->addCollider();
	enemy->getColider()->set_size({2,2,1});
	enemy->getColider()->set_shift({1,1,0.5});

	//obj->addSpriteAnimation(SpriteAnimation(4,8,500));
	player->addScript(currScene, &routine);
	player->addCollider();
	player->addRigidBody();
	player->getColider()->set_size({1,1,1});
	player->getColider()->set_shift({0.5,0.5,0.5});

    /*for (int i = 0; i < 1; i += 1) {
        auto* op = currScene->createObject("pistol " + std::to_string(i), glm::vec3{ i * 2,i,0 }, glm::vec3{ 1,1,1 }, glm::vec3{ 1,1,3 }, GameState::engine_path + "meshes/pistol/homemade_lasergun_upload.obj",
            sv, currShaderRoutine, currScene, &routine, false, false);
        op->frozeObject();
        op->setMaterial(Material(32));
        //op -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1)));
    }
	*/
    //danceAnimation = Animation("../../../meshes/animations/bot/reach.dae", &ourModel);
}

void Renderer::render(Window* wind, bool& debug_mode) {

    glfwPollEvents();
    int display_w, display_h;
    glfwGetFramebufferSize(wind->getWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    //glDisable(GL_FRAMEBUFFER_SRGB);

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(sv.getProgram());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    currScene->renderScene();

    if (GameState::cam.cursor_hidden) {
        glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
        glm::mat4  view = GameState::cam.getBasicLook();
        sv.setMat4("projection", projection);
        sv.setMat4("view", view);
    }

    //TODO(darius) make it faster. Instanced rendering? Batching?
    currScene->renderParticles();
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    currScene->updateAnimators(deltaTime);//TODO(darius) check if heres bug with time step instead fo time value
    currScene->updateSpriteAnimations(static_cast<float>(glfwGetTime()));
	currScene->updateScene();

    if (GameState::cam.cursor_hidden) {
        glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
        glm::mat4  view = GameState::cam.getBasicLook();
        dbr.updateCamera(projection, view);
    }

    if (debug_mode)
    {
        //render debug coiders
        for (int i = 0; i < currScene->get_objects().size(); ++i) {
            if (currScene->get_objects()[i]) {
                dbr.renderDebugColider(wind, currScene->get_object_at(i)->getColider(),
                    currScene->get_object_at(i)->getRigidBody());
                dbr.renderDebugLightSource(currScene->get_objects()[i]->getPointLight());
                currScene->get_objects()[i]->traverseObjects([&dbr = dbr](Object* obj) {
                    dbr.renderDebugLightSource(obj->getPointLight());
                    });

                //dbr.renderDebugLine(currScene->get_objects()[i]->getTransform().position, {1,1,1});
            }
        }
        //dbr.renderPoints();
        dbr.renderDebugGrid();
    }
}

void Renderer::updateBuffers(Window* wind)
{
    glfwSwapBuffers(wind->getWindow());
}

size_t Renderer::getShaderRoutine()
{
    return sv.getProgram();
}

LightingShaderRoutine& Renderer::getCurrShaderRoutine()
{
    return currShaderRoutine;
}

Shader Renderer::getShader()
{
    return sv;
}

Scene* Renderer::getScene()
{
	return currScene;
}

DebugRenderer& Renderer::getDebugRenderer()
{
    return dbr;
}
