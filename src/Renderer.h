#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>

#include "Window.h"
#include "GameState.h"
#include "Shader.h"
#include "InputManager.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"
#include "EBO.h"
#include "Model.h"
#include <CubeMesh.h>
#include <Scene.h>
#include <Transform.h>
#include <UI.h>
#include <ParticleSystem.h>
#include <Script.h>
#include <Color.h>
#include <PointLight.h>

/*
TODO(all):
	code:
        - move and drag - subentities
        - multithreaded objects loading
        - debug thread
        - benchmark
        - camera as object
        - mouse clickable objects
        - entities destruction
        - scene serialization(JSON(try simdjson eventually?) and binary)

    renderer:
        - better renderer
        - matrials
        - mesh optimization - https://github.com/zeux/meshoptimizer
	
    Physics:
        - Basic Collission detection and Physics         v
        - Inertia Matrix physics                         v
        - Particles system                               v
        - Soft Bodyies
        - Liquid Simulation 
        - Cloth Simulation
        - Hair Simulation

    imgui:
        - Panel system
        - general menu
        - show file system traversal
        - better properties traverse

    refs:
        - unity coliders - https://www.youtube.com/watch?v=bh9ArKrPY8w&ab_channel=Unity
        - titanfall SIMD collisions - https://www.youtube.com/watch?v=6BIfqfC1i7U&t=81s&ab_channel=GDC
        - GJK explained - https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
        - Minkowski diff by Casey - https://www.youtube.com/watch?v=_g8DLrNyVsQ&t=2933s&ab_channel=MollyRocket
        - Gregory - http://ce.eng.usc.ac.ir/files/1511334027376.pd
        - RigidBody Dynamics - https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf
        - Quaternions - https://www.3dgep.com/understanding-quaternions/
        - Hair - https://github.com/hankeyyh/FurSim/tree/master
        - Grass - https://www.youtube.com/watch?v=Ibe1JBF5i5Y&t=0s&ab_channel=GDC
        - Vertex optimisation - https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html
        - Mikkelsen - https://gamedev.stackexchange.com/questions/146855/how-do-you-compute-the-tangent-space-vectors-with-normals-given-in-the-mesh
        - GPU RigidBody - https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-29-real-time-rigid-body-simulation-gpus
        - Physics - https://theswissbay.ch/pdf/Gentoomen%20Library/Game%20Development/Programming/Game%20Physics%20Engine%20Development.pdf
        - particles - https://www.youtube.com/watch?v=G6OGKP3MaUI&list=PLnuhp3Xd9PYTt6svyQPyRO_AAuMWGxPzU&index=168&ab_channel=MollyRocket
        - Geometry - https://www.geometrictools.com/Source/Mathematics.html
        - https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method
        - https://raphaelpriatama.medium.com/sequential-impulses-explained-from-the-perspective-of-a-game-physics-beginner-72a37f6fea05
        - "Predictive contacts"
        - "Inastanced rendering"
        - https://hitokageproduction.com/article/11
        - https://gafferongames.com/post/collision_response_and_coulomb_friction/
        - https://www.youtube.com/watch?v=DGVZYdlw_uo&ab_channel=kuju
        - AOE networking - https://www.gamedeveloper.com/programming/1500-archers-on-a-28-8-network-programming-in-age-of-empires-and-beyond
        - Porting - https://gamedev.stackexchange.com/questions/103941/porting-sdl-opengl-game-to-android-and-ios
        - Net - https://web.archive.org/web/20210419133753/https://gameserverarchitecture.com/
        - Android port - https://arm-software.github.io/opengl-es-sdk-for-android/tutorials.html
                         https://keasigmadelta.com/store/wp-content/uploads/2017/03/GLES3-and-SDL2-Tutorials.pdf
*/

class DebugRenderer 
{
public:
    DebugRenderer() : dsv("../../../shaders/debugVertexShader.glsl", GL_VERTEX_SHADER), dsf("../../../shaders/debugFragmentShader.glsl", GL_FRAGMENT_SHADER)
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

    void setupSceneGrid()
    {
		for (int i = 0; i <= slices; ++i) {
            for (int j = 0; j <= slices; ++j) {
                float x = (float)j;// / (float)slices;
                float y = 0;
                float z = (float)i;// / (float)slices;
                vertices_grid.push_back(glm::vec3((x-slices/2) * grid_scale, (y) * grid_scale, (z-slices/2) * grid_scale));
            }
        }

        for (int i = 0; i < slices; ++i) {
            for (int j = 0; j < slices; ++j) {
                int row1 = j * (slices + 1);
                int row2 = (i + 1) * (slices + 1);

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

    void renderDebugColider(Window* wind, std::optional<Colider>& collider, std::optional<RigidBody>& body)
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
        glBindVertexArray(0)    ;
    }

    void renderDebugGrid()
    {
		glUseProgram(dsv.getProgram());
        auto model = glm::mat4(1.0f);
        dsv.setVec4("objectColor", {0.4,0.4,0.4,0});
        dsv.setMat4("model", model);
        vao_grid.bind();
        glDrawElements(GL_LINES, length, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }

    void renderDebugLightSource(std::optional<PointLight>& p)
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

    void updateCamera(glm::mat4 projection, glm::mat4 view)
    {
        glUseProgram(dsv.getProgram());
		dsv.setMat4("projection", projection);
        dsv.setMat4("view", view);
    }

private:
    size_t length = 0;
    size_t slices = 100;
	std::vector<glm::vec3> vertices_grid;
	std::vector<glm::uvec4> indices_grid;
    int grid_scale = 1;

    //TODO(darius) use cubeMesh
    float vertices[48 * 6] = {
		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 

		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 

		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 

		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
    };

    unsigned int indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    VBO vbo;

    VBO lightVbo;

    VAO vao;

    VAO vao_grid;

    EBO ebo;

    Shader dsv;
    Shader dsf;
};

class Renderer
{
public:
    Renderer(Scene* currScene_in) : currScene(currScene_in), sv("../../../shaders/vertexShader.glsl", GL_VERTEX_SHADER), sf("../../../shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER) {
        pointLight = PointLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1));
        directionalLight = DirectionalLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1));
        directionalLight.ambient = {0,0,0};
        spotLight = SpotLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(0,-1,0));

        objectMaterial.ambient = {1,0,1};
        objectMaterial.diffuse = {1,0,1};
        objectMaterial.specular = {1,0,1};
        objectMaterial.shininess = 32;

        //glfwSetCursorPos(wind->getWindow(), wind->getWidth() / 2, wind->getHeight() / 2);

        sv.compile();
        sf.compile();
        sv.link(sf);

        auto currShaderRoutine = [sv = this -> sv, &directionalLight = directionalLight, &pointLight = pointLight, &objectMaterial = this -> objectMaterial]
        (Transform tr) {
            sv.setVec3("viewPos", GameState::cam.getCameraPos());
            sv.setInt("lightsCount", PointLight::LightsCount);

            directionalLight.setShaderLight(sv);
            pointLight.setShaderLight(sv);
            //spotLight.setShaderLight(sv);
            objectMaterial.setShaderMaterial(sv);

            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 pos = tr.position;
            glm::mat4 q = tr.get_quatmat();
            glm::vec3 scale = tr.scale;

            model = glm::translate(model, pos);
            model *= q;
            model = glm::scale(model, scale);

            sv.setMat4("model", model);
        };

        //TODO(darius) make something like ScriptCode class and load update anmd setup from precompilde .o file. But we need separate lib for user
        auto objSetupRoutine = [&directionalLight = directionalLight, &pointLight = pointLight,&spotLight = spotLight, &objectMaterial = this -> objectMaterial](ScriptArgument* args) {
            Object* obj = args->obj;
            Script* scr = args->script;
            float vectorv = 0;
            scr -> addVectorProperty(&(directionalLight.direction), "directional light direction");
            scr -> addVectorProperty(&(directionalLight.ambient), "directional light ambient");
            scr -> addVectorProperty(&(directionalLight.diffuse), "directional light diffuse");
            scr -> addVectorProperty(&(directionalLight.specular), "directional light specular");

            scr -> addVectorProperty(&(pointLight.position), "point light position");
            scr -> addVectorProperty(&(pointLight.ambient), "point light ambient");
            scr -> addVectorProperty(&(pointLight.diffuse), "point light diffuse");
            scr -> addVectorProperty(&(pointLight.specular), "point light specular");
            scr -> addFloatProperty(&(pointLight.linear), "point light linear");
            scr -> addFloatProperty(&(pointLight.quadratic), "point light quadratic");

            std::cout << "start\n";
            int max = 5;
            int min = -5;
            int v1 = (rand() % (max - min)) + min;
            int v2 = (rand() % (max - min)) + min;
            int v3 = (rand() % (max - min)) + min;

            //obj->getRigidBody().reset();
            obj->apply_force({0,-1,0});
			//obj->getRigidBody().get_is_static_ref() = true;
			//obj->getRigidBody().add_angular_force({ 1,0,0 });
			//obj->getRigidBody().create_box_inertia_tensor(1, { 1,1,1 });

            obj->traverseObjects([args](Object* obj){
                if(!obj->getRigidBody())
                    return;
				obj->getRigidBody()->get_is_static_ref() = true;
				//obj->getRigidBody().add_angular_force({ 5,0,0 });
                obj->apply_force({0,-1,0});
				obj->getRigidBody()->create_box_inertia_tensor(1, { 1,1,1 });
            });

			GameState::debug_msg.append("setup complete for " + obj -> get_name() + "\n");
        };

        auto objSetupRoutine2 = [](ScriptArgument* args) {
            Object* obj = args->obj;
            Script* scr = args->script;

            std::cout << "start\n";
            int max = 5;
            int min = -5;
            int v1 = (rand() % (max - min)) + min;
            int v2 = (rand() % (max - min)) + min;
            int v3 = (rand() % (max - min)) + min;

            //obj->getRigidBody().reset();
            obj->apply_force({0,1,0});
            //obj->getRigidBody().get_is_static_ref() = true;
            //obj->getRigidBody().add_angular_force({ 1,0,0 });
            //obj->getRigidBody().create_box_inertia_tensor(1, { 1,1,1 });

            obj->traverseObjects([](Object* obj){
                    
                obj->getRigidBody()->get_is_static_ref() = true;
                obj->getRigidBody()->add_angular_force({ 1,0,0 });
                obj->getRigidBody()->create_box_inertia_tensor(1, { 1,1,1 });
            });

            GameState::debug_msg.append("setup complete for " + obj -> get_name() + "\n");
        };

        auto objUpdateRoutine = [](ScriptArgument* args) {
			//GameState::debug_msg.append("update for " + obj -> get_name() + "\n");
            return; 
        };

        for(int i = 0; i < 1; i += 1){
          auto* op = currScene->createObject("pistol " + std::to_string(i), glm::vec3{ i * 2,i + 5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{1,1,3}, "../../../meshes/pistol/homemade_lasergun_upload.obj", 
                sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
            op -> frozeObject();
            //op -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1)));
        }
        auto* op = currScene->createObject("light1", glm::vec3{ 5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{0,0,0}, ".obj", 
                                            sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
        op -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1)));
        op = currScene->createObject("light2", glm::vec3{ 5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{0,0,0}, ".obj", 
                                            sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
        
        op -> addPointLight(PointLight(glm::vec3{-0.2f, 1.0f, -0.3f}, glm::vec3(1,1,1)));
        op = currScene->createObject("light3", glm::vec3{ 5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{0,0,0}, ".obj", 
                                            sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
        
        op -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, 0.3f}, glm::vec3(1,1,1)));
        op = currScene->createObject("light4", glm::vec3{ 5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{0,0,0}, ".obj", 
                                            sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
        
        op -> addPointLight(PointLight(glm::vec3{0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1)));
        op = currScene->createObject("light5", glm::vec3{ 5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{0,0,0}, ".obj", 
                                            sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine, false, false);
        
        op -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, 0.3f}, glm::vec3(1,1,1)));


        auto* simpleLight = currScene->createObject("cimple light", sv, currShaderRoutine);
        simpleLight -> addPointLight(PointLight(glm::vec3{-0.2f, -1.0f, 0.3f}, glm::vec3(1,1,1)));
 
       
        auto* ob = currScene->createObject("backpackEntity", glm::vec3{-1,-13,1}, glm::vec3{ 1,1,1 }, glm::vec3{2,2,2}, "", sv, currShaderRoutine, currScene, objSetupRoutine, objUpdateRoutine);
        auto* entt = currScene->createEntity(ob,"../../../meshes/backpack/backpack.obj", sv, currShaderRoutine, true);


        cube.setDrawMode(1);

        /*particles.addParticle(Model("../../../meshes/backpack/backpack.obj", sv, currShaderRoutine), {});
        for(int i = 0; i < 10; ++i){
            particles.addPosition({i,i,i});
        } 
        */
    }

    void render(Window* wind, bool& debug_mode) {
        glfwPollEvents();
        int display_w, display_h;
        glfwGetFramebufferSize(wind->getWindow(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
 
        particles.updateUniform3DDistribution(glfwGetTime());
        particles.renderParticles();

        if (GameState::cam.cursor_hidden) {
            glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
            glm::mat4  view = GameState::cam.getBasicLook();
            dbr.updateCamera(projection, view);
        }

        if (debug_mode)
        {
            //render debug coiders
            for (int i = 0; i < currScene->get_objects().size(); ++i){
                if (currScene->get_objects()[i]){
                    dbr.renderDebugColider(wind, currScene->get_object_at(i)->getColider(),
                                                 currScene->get_object_at(i)->getRigidBody());
                    dbr.renderDebugLightSource(currScene->get_objects()[i]->getPointLight()); 
                }
            }
            dbr.renderDebugGrid();

        }

        //glfwSwapBuffers(wind->getWindow());
    }

    void updateBuffers(Window* wind)
    {
        glfwSwapBuffers(wind->getWindow());
    }

private:
    DebugRenderer dbr;
    Shader sv;
    Shader sf;

    CubeMesh cube;
    ParticleSystem particles;

    Scene* currScene;

    Material objectMaterial;

    PointLight pointLight;
    DirectionalLight directionalLight;
    SpotLight spotLight;
};
