#include "Window.h"
#include "InputManager.h"

#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



Window::Window(char* windowName, int w, int h) : width(w), height(h), m_window_name(windowName)
{
	//TODO(darius) i dont want to create context each time i create window
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_window = glfwCreateWindow(width, height, m_window_name, nullptr, nullptr);
	
	if(m_window == nullptr)
	{
		std::cerr << "cant create window\n";
		glfwTerminate(); 
		return;
	}else{
		//std::cout << "can create window\n";	
	}

	glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, InputManager::framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, InputManager::mouse_callback);
    glfwSetScrollCallback(m_window, InputManager::scroll_callback);
    glfwSetDropCallback(m_window, InputManager::drop_callback);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(m_window, InputManager::framebuffer_size_standart_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw std::runtime_error("can not init glad");
	}

	int fbWidth = 0, fbHeight = 0;
	glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
	width = fbWidth;
	height = fbHeight;
	glViewport(0, 0, fbWidth, fbHeight);
}

Window::~Window()
{
	glfwTerminate();
}

bool Window::isShouldClose() {
	return glfwWindowShouldClose(m_window);
}

void Window::setShouldClose(bool flag) {
	glfwSetWindowShouldClose(m_window, flag);
}

void Window::showWindow()
{
}

GLFWwindow* Window::getWindow() {
	return m_window;//not cool
}

int Window::getWidth() {
	return width;
}

int Window::getHeight() {
	return height;
}

void Window::setWidth(int W) {
	width = W;
}

void Window::setHeight(int H) {
	height = H;
}

void Window::swapBuffers() {
	glfwSwapBuffers(m_window);
}

