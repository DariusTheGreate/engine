#pragma once
#include <functional>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class InputManager
{
public:
	//InputManager(std::function<void()> pi, std::function<void()> fbc) : processInput(pi), frame_buffer_callback(fbc) {}
	static void processInput(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
private:
	//std::function<void()> processInput;
	//std::function<void()> frame_buffer_callback;
};

