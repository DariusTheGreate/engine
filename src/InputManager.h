#pragma once
#include <functional>
#include <iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "GameState.h" 

class InputManager
{
public:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	static void standart_input_processor(GLFWwindow* window);

	static void framebuffer_size_standart_callback(GLFWwindow* window, int width, int height);

    static void processInput(GLFWwindow* window);

    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
