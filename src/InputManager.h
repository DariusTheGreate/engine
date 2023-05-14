#pragma once
#include <functional>
#include <iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "GameState.h" 

class InputManager
{
public:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}


	static void standart_input_processor(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	static void framebuffer_size_standart_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

    static void processInput(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            GameState::ks.set_w(true);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            GameState::ks.set_s(true);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            GameState::ks.set_a(true);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            GameState::ks.set_d(true);
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            GameState::ks.set_0(true);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            GameState::ks.set_1(true);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            GameState::ks.set_2(true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
            GameState::ks.set_w(false);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
            GameState::ks.set_s(false);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
            GameState::ks.set_a(false);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
            GameState::ks.set_d(false);
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
            GameState::ks.set_0(false);
        }
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            GameState::ks.set_1(false);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            GameState::ks.set_2(false);
        }
    }

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        GameState::ms.set_x((int)xposIn);
        GameState::ms.set_y((int)yposIn);
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        GameState::ms.set_offset_x((int)xoffset);
        GameState::ms.set_offset_y((int)(yoffset));
    }
};

