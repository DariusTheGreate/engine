#pragma once

#include <functional>
#include <iostream>

#include "Window.h"
#include "GameState.h"
#include "Shader.h"
#include "InputManager.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Texture.h"

class Renderer
{
public:
    Renderer(Window* wind) : sv("shaders/vertexShader.glsl", GL_VERTEX_SHADER), sf("shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER), t1("textures/putin2.jpg", GL_RGB, GL_RGB),
        lightColor(0.0f, 1.0f, 1.0f), objectColor(0.9f, 0.4f, 0.4f) {

        glfwSetCursorPos(wind->getWindow(), wind->getWidth() / 2, wind->getHeight() / 2);
        GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
        
        sv.compile();

        sf.compile();
        curr_shader_program = sv.link(sf);

        vao.bind();

        vbo.bind(vertices);
        vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        vbo.vboEnableVertexAttribArray(0);

        vbo.setVAO(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        vbo.vboEnableVertexAttribArray(1);

        vbo.setVAO(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        vbo.vboEnableVertexAttribArray(2);

        ebo.bind(indices);
    }

    //window passing is cringe! supposed to be private
	void render(Window* wind) {
        updateInput();
        updateGame();
        glEnable(GL_DEPTH_TEST);

        t1.activate(GL_TEXTURE1);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(curr_shader_program);
        
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        
        glm::mat4 projection = GameState::cam.getPerspective(wind->getWidth(), wind->getHeight());
        sv.setMat4("projection", projection, curr_shader_program);

        glm::mat4  view = GameState::cam.getBasicLook();
        sv.setMat4("view", view, curr_shader_program);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            sv.setMat4("model", model, curr_shader_program);
            sv.setVec3("lightColor", lightColor, curr_shader_program);
            sv.setVec3("objectColor", objectColor, curr_shader_program);
            sv.setVec3("lightPos", GameState::cam.getCameraPos(), curr_shader_program);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(wind->getWindow());
        glfwPollEvents();
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

        GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
        GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
    }

    void setPolygonMode() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void updateGame() {
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 1.0f;

        //lightColor = glm::vec3(0, greenValue, greenValue);
    }
	
private:
    Shader sv;
    Shader sf;
	bool render_me = false;
    unsigned int curr_shader_program;
    unsigned int light_shader_program;
    
    //move it into Mesh class
    float vertices[48 * 6] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f
    };

    float lightVertices[6*18] = {
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
    -0.5f,  0.5f, -0.5f
    };

    glm::vec3 cubePositions[10] = {
        glm::vec3(0.0f,  0.0f,  -1.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 lightColor, objectColor;

    unsigned int indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    VBO<sizeof(vertices)> vbo;

    VBO<sizeof(lightVertices)> lightVbo;

    VAO vao;

    EBO<sizeof(indices)> ebo;

    Texture t1;
};

