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
            GameState::cam.moveCameraForward();
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            GameState::cam.moveCameraBackward();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            GameState::cam.moveCameraLeft();
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            GameState::cam.moveCameraRight();
    }

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
    {
        GameState::cam.setCameraLook(xposIn, yposIn);
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        GameState::cam.scroll_callback(xoffset, yoffset);
    }
};

