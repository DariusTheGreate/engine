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
#include <Scene.h>
#include <Transform.h>
#include <UI.h>

/*
    TODO:

    refs:
    https://github.com/pkurth/D3D12Renderer/tree/master

    code:
		- multithreaded objects loading
		- debug thread
		- benchmark
		- rigid body using quaternions and inertia matrix
		- collisions using GJK-algorithm
		- camera as object

    renderer:
		- debug renderer
		- collisions rendering
		- arrows renedring
		- better renderer
		- meshes?

    imgui:
		- show file system traversal  
		- console
		- general menu

*/

class Renderer
{
public:
    Renderer(Window* wind) : window(wind -> getWindow()), ui(wind -> getWindow()), sv("../../../shaders/vertexShader.glsl", GL_VERTEX_SHADER), sf("../../../shaders/directionalLightFragmentShader.glsl", GL_FRAGMENT_SHADER), 
        dsv("../../../shaders/debugVertexShader.glsl", GL_VERTEX_SHADER), dsf("../../../shaders/debugFragmentShader.glsl", GL_FRAGMENT_SHADER),
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
            obj->getRigidBody().get_is_static_ref() = true;
            obj->getRigidBody().add_angular_force({1,0,0});
            //obj->apply_force({v1,v2,v3});
			obj -> getRigidBody().create_box_inertia_tensor(1, {1,1,1 });
        };
		 
        auto objUpdateRoutine = [](void* scene_handler, void* object_handler) {
            Object* obj = reinterpret_cast<Object*>(object_handler);
            Scene* scene = reinterpret_cast<Scene*>(scene_handler);
            return;

			if (obj -> getRigidBody().get_pos().x > 50 || obj -> getRigidBody().get_pos().x < -20) {
				obj -> getRigidBody().reset();
			}

			if (obj -> getRigidBody().get_pos().y > 50 || obj -> getRigidBody().get_pos().y < -20) {
				obj -> getRigidBody().reset();
			}
			
            if (obj -> getRigidBody().get_pos().z > 50 || obj -> getRigidBody().get_pos().z < -20) {
				obj -> getRigidBody().reset();
			}
        };

        auto simpleObjSetupRoutine = [](void* scene_handler, void* object_handler) {
            Object* obj = reinterpret_cast<Object*>(object_handler);
            Scene* scene = reinterpret_cast<Scene*>(scene_handler);
            obj->getRigidBody().get_is_static_ref() = true;
        };

        //for(int i = 0; i < 10; i += 1)
		//	currScene.createObject("pistol " + std::to_string(i), glm::vec3{i * 2,3,0}, glm::vec3{1,1,1}, "../../../meshes/pistol/homemade_lasergun_upload.obj", sv, currShaderRoutine, &currScene, objSetupRoutine, objUpdateRoutine);

        //currScene.createObject(glm::vec3{ 5,0,0 }, glm::vec3{ 0.1,0.1,0.1 }, "../meshes/frozegun.obj", sv, currShaderRoutine);
        currScene.createObject("your mother", glm::vec3{ 0,0,0}, glm::vec3{ 0.01,0.01,0.01 }, "../../../meshes/Deadpool/d.obj", sv, currShaderRoutine, &currScene, simpleObjSetupRoutine, simpleObjSetupRoutine, false, true);

        dsv.compile();
        dsf.compile();
        curr_dbg_shader_program = sv.link(dsf);
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
        currScene.renderScene();
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
            setPolygonMode();
        }

        GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
        GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
    }

    void setPolygonMode() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
    Shader sv;
    Shader sf;

    GLFWwindow* window;

    Shader dsv;
    Shader dsf;
    UI ui;

    Scene currScene;

	bool render_me = false;
    unsigned int curr_shader_program;
    unsigned int curr_dbg_shader_program;
    unsigned int frame_number = 0;
    double lastTime;

    bool cursor_hidden = true;

    bool show_demo_window = false;
    bool show_another_window = false;

    glm::vec3 lightColor, objectColor, lightPos;
};

