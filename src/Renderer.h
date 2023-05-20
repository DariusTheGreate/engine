#pragma once

#include <functional>
#include <iostream>

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

/*
    TODO:
	code:
        - fix entity orientation
        - move subentities

        - multithreaded objects loading
        - debug thread
        - benchmark
        - camera as object
        - mouse clickable objects
        - entities destruction
        - scene serialization(JSON(try simdjson eventually?) and binary)

    renderer:
        - debug renderer
        - collisions rendering
        - better renderer
        - meshes?
	
    Physics:
        - Basic Collission detection and Physics
        - Inertia Matrix physics
        - Particles system
        - Soft Bodyies
        - Liquid Simulation 
        - Cloth Simulation
        - Hair Simulation

    imgui:
        - Guizmos
        - Panel system
        - collider property
        - script property
        - draging!
        - console
        - general menu
        - show file system traversal

    refs:
        - engine - https://github.com/pkurth/D3D12Renderer/tree/master
        - unity coliders - https://www.youtube.com/watch?v=bh9ArKrPY8w&ab_channel=Unity
        - titanfall SIMD collisions - https://www.youtube.com/watch?v=6BIfqfC1i7U&t=81s&ab_channel=GDC
        - GJK explained - https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
        - Minkowski diff by Casey - https://www.youtube.com/watch?v=_g8DLrNyVsQ&t=2933s&ab_channel=MollyRocket
        - Gregory - http://ce.eng.usc.ac.ir/files/1511334027376.pdf
        - RigidBody Dynamics - https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf
        - Guizmo Tools Lib - https://github.com/CedricGuillemet/ImGuizmo
        - Quaternions - https://www.3dgep.com/understanding-quaternions/
        - Hair - https://github.com/hankeyyh/FurSim/tree/master
        - Grass - https://www.youtube.com/watch?v=Ibe1JBF5i5Y&t=0s&ab_channel=GDC
        - Vertex optimisation - https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html
        - Mikkelsen - https://gamedev.stackexchange.com/questions/146855/how-do-you-compute-the-tangent-space-vectors-with-normals-given-in-the-mesh
        - GPU RigidBody - https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-29-real-time-rigid-body-simulation-gpus
        - Physics - https://theswissbay.ch/pdf/Gentoomen%20Library/Game%20Development/Programming/Game%20Physics%20Engine%20Development.pdf
        - particles - https://www.youtube.com/watch?v=G6OGKP3MaUI&list=PLnuhp3Xd9PYTt6svyQPyRO_AAuMWGxPzU&index=168&ab_channel=MollyRocket
*/

//Guizmmos here
class DebugRenderer 
{
public:
    DebugRenderer() : dsv("../../../shaders/debugVertexShader.glsl", GL_VERTEX_SHADER), dsf("../../../shaders/debugFragmentShader.glsl", GL_FRAGMENT_SHADER)
    {
		dsv.compile();
        dsf.compile();
        curr_dbg_shader_program = dsv.link(dsf);

        vbo.init();
        ebo.init();
        vao.init();

        vao.bind();

        vbo.bind(48 * 6 * sizeof(float), vertices);
        vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        vbo.vboEnableVertexAttribArray(0);
        ebo.bind(6 * sizeof(unsigned int), indices);
    }

    void renderDebugColider(Window* wind, glm::vec3 position, glm::vec3 scale, glm::mat4 q)
    {
        //DEBUG
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
        glUseProgram(curr_dbg_shader_program);

        glm::mat4 model = glm::mat4(1.0f);
        //std::cout << position.x << "|" << position.y << "|" << position.z << "\n";
        model = glm::translate(model, position);
        model *= q;
        model = glm::scale(model, scale);

        dsv.setMat4("model", model, curr_dbg_shader_program);
        vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);//IMPORTANT
    }

    void updateCamera(glm::mat4 projection, glm::mat4 view)
    {
        glUseProgram(curr_dbg_shader_program);
		dsv.setMat4("projection", projection, curr_dbg_shader_program);
        dsv.setMat4("view", view, curr_dbg_shader_program);
    }

private:
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

    EBO ebo;

    Shader dsv;
    Shader dsf;

    unsigned int curr_dbg_shader_program;
};

class Renderer
{
public:
    Renderer(Window* wind) : window(wind -> getWindow()), ui(wind -> getWindow()), sv("../../../shaders/vertexShader.glsl", GL_VERTEX_SHADER), sf("../../../shaders/directionalLightFragmentShader.glsl", GL_FRAGMENT_SHADER),
        lightColor(0.0f, 1.0f, 1.0f), objectColor(0.9f, 0.4f, 0.4f), lightPos(glm::vec3(0.0f, 0.0f, 15.0f)) {

        glfwSetCursorPos(wind->getWindow(), wind->getWidth() / 2, wind->getHeight() / 2);
        //setPolygonMode();
        GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);

        lastTime = glfwGetTime();

        sv.compile();
        sf.compile();
        curr_shader_program = sv.link(sf);

        auto currShaderRoutine = [sv = this -> sv, lightPos = this -> lightPos, lightColor = this -> lightColor, objectColor = this -> objectColor, curr_shader_program = this -> curr_shader_program]
        (Transform tr) {
            sv.setVec3("lightColor", lightColor, curr_shader_program);
            sv.setVec3("objectColor", objectColor, curr_shader_program);
            sv.setVec3("lightPos", lightPos, curr_shader_program);

            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 pos = tr.position;
            glm::mat4 q = tr.get_quatmat();
            glm::vec3 scale = tr.scale;

            model = glm::translate(model, pos);
            model *= q;
            model = glm::scale(model, scale);

            sv.setMat4("model", model, curr_shader_program);
        };

        //currScene.createObject(glm::vec3{5,0,0 }, "../meshes/backpack/backpack.obj", sv, currShaderRoutine, false, true, 0);

        auto objSetupRoutine = [](void* scene_handler, void* object_handler) {
            Object* obj = reinterpret_cast<Object*>(object_handler);
            Scene* scene = reinterpret_cast<Scene*>(scene_handler);

            std::cout << "start\n";
            int max = 5;
            int min = -5;
            int v1 = (rand() % (max - min)) + min;
            int v2 = (rand() % (max - min)) + min;
            int v3 = (rand() % (max - min)) + min;

            //obj->getRigidBody().reset();
            //obj->apply_force({v1,v2,v3});
            obj->traverseObjects([](Object* obj){
					
				obj->getRigidBody().get_is_static_ref() = true;
				obj->getRigidBody().add_angular_force({ 1,0,0 });
				obj->getRigidBody().create_box_inertia_tensor(1, { 1,1,1 });
            });

			GameState::debug_msg.append("setup complete for " + obj -> get_name() + "\n");
        };

        auto objUpdateRoutine = [](void* scene_handler, void* object_handler) {
            Object* obj = reinterpret_cast<Object*>(object_handler);
            Scene* scene = reinterpret_cast<Scene*>(scene_handler);
            return;

            if (obj->getRigidBody().get_pos().x > 50 || obj->getRigidBody().get_pos().x < -20) {
                obj->getRigidBody().reset();
            }

            if (obj->getRigidBody().get_pos().y > 50 || obj->getRigidBody().get_pos().y < -20) {
                obj->getRigidBody().reset();
            }

            if (obj->getRigidBody().get_pos().z > 50 || obj->getRigidBody().get_pos().z < -20) {
                obj->getRigidBody().reset();
            }
        };

        auto simpleObjSetupRoutine = [](void* scene_handler, void* object_handler) {
            Object* obj = reinterpret_cast<Object*>(object_handler);
            Scene* scene = reinterpret_cast<Scene*>(scene_handler);
            obj->getRigidBody().get_is_static_ref() = true;
        };

        //for(int i = 0; i < 2; i += 1)
        //    currScene.createObject("pistol " + std::to_string(i), glm::vec3{ i * 2,i + 5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{1,1,3}, "../../../meshes/pistol/homemade_lasergun_upload.obj", sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine, false, false);

		//currScene.createObject("backpack" + std::to_string(1), glm::vec3{5,5,0 }, glm::vec3{ 1,1,1 }, glm::vec3{1,1,3}, "../../../meshes/backpack/backpack.obj", sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine, false, true);
        auto* ob = currScene.createObject("backpackEntity", glm::vec3{-1,-13,1}, glm::vec3{ 1,1,1 }, glm::vec3{2,2,2}, "", sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine);
        auto* entt = currScene.createEntity(ob,"../../../meshes/backpack/backpack.obj", sv, currShaderRoutine, true);

        cube.setDrawMode(1);

        auto* cubeMeshObject1 = currScene.createObject("cube", glm::vec3{2,2,0}, glm::vec3{1,1,1}, glm::vec3{1,1,1}, cube, sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine);
        auto* cubeMeshObject2 = currScene.createObject("cube2", glm::vec3{0,0,0}, glm::vec3{1,1,1}, glm::vec3{0.99, 0.99, 0.99}, cube, sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine);
        auto* cubeMeshObject3 = currScene.createObject("cube3", glm::vec3{1,1,0}, glm::vec3{1,1,1}, glm::vec3{0.99, 0.99, 0.99}, cube, sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine);
    }

    void render(Window* wind) {
        updateInput();
        updateGame();
        glfwPollEvents();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(curr_shader_program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
		if (cursor_hidden || GameState::cam.getUnexpectedUpdate()) {
			glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
			glm::mat4  view = GameState::cam.getBasicLook();

		    sv.setMat4("projection", projection, curr_shader_program);
            sv.setMat4("view", view, curr_shader_program);

            if (GameState::cam.getUnexpectedUpdate())
                GameState::cam.setUnexpectedUpdate(false);
        }

        //cube.Draw(sv, curr_shader_program);

        currScene.renderScene();

        if (cursor_hidden) {
            glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
            glm::mat4  view = GameState::cam.getBasicLook();
            dbr.updateCamera(projection, view);
        }

        if (debug_mode)
        {
            //render debug coiders
            for (int i = 0; i < currScene.get_objects().size(); ++i)
                if (currScene.get_objects()[i])
                    dbr.renderDebugColider(wind, currScene.get_object_at(i)->getColider().get_render_start_point(), currScene.get_object_at(i)->getColider().get_size(),
                                                 currScene.get_object_at(i)->getRigidBody().tr.get_quatmat());
        }
        
        ui.renderUI(currScene);
        ui.apply();

        glfwSwapBuffers(wind->getWindow());
    }

    void updateInput() {
        if (GameState::ks.get_w()) {
            GameState::cam.moveCameraForward();
        }
        if (GameState::ks.get_a()) {
            GameState::cam.moveCameraLeft();
        }
        if (GameState::ks.get_s()) {
            GameState::cam.moveCameraBackward();
        }
        if (GameState::ks.get_d()) {
            GameState::cam.moveCameraRight();
        }
        if (GameState::ks.get_2()) {
            cursor_hidden = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (GameState::ks.get_1()) {
            cursor_hidden = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (GameState::ks.get_0()) {
            debug_mode = true;
            setPolygonMode(GL_LINE);
        }
        if (GameState::ks.get_9()) {
            debug_mode = false;
            setPolygonMode(GL_FILL);
        }

        GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
        GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
    }

    void setPolygonMode(size_t type = GL_LINE) {
        glPolygonMode(GL_FRONT_AND_BACK, type);
    }

    void updateGame() {
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 1.0f;
        lightColor = glm::vec3(0, greenValue, greenValue);
    }

    void printFPS() {
        double currentTime = glfwGetTime();
        frame_number++;
        if (currentTime - lastTime >= 1.0) {
            std::cout << "framerate: " << 1000.0 / double(frame_number);
            frame_number = 0;
            lastTime += 1.0;
        }
    }

private:
    DebugRenderer dbr;
    Shader sv;
    Shader sf;

    CubeMesh cube;

    GLFWwindow* window;
    UI ui;

    Scene currScene;

    bool render_me = false;
    unsigned int curr_shader_program;

    unsigned int frame_number = 0;
    double lastTime = 0;

    bool cursor_hidden = true;
    bool debug_mode = false;

    bool show_demo_window = false;
    bool show_another_window = false;

    glm::vec3 lightColor, objectColor, lightPos;

  };
