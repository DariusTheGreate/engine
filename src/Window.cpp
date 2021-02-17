#include "Window.h"

Window::Window(char* windowName, int w, int h)
{
	width = w;
	height = h;
	m_window_name = windowName;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(w, h, windowName, nullptr, nullptr);
	
	if(m_window == nullptr)
	{
		std::cerr << "cant create window\n";
		glfwTerminate(); 
		return;
	}
	glfwMakeContextCurrent(m_window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		std::cerr << "cant init glew\n";
		return;
	}

	glViewport(0,0,w,h);
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

void Window::swapBuffers() {
	glfwSwapBuffers(m_window);
}