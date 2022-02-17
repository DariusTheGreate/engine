#ifndef _WINDOW_
#define _WINDOW_

#include <iostream>
//#define GLEW_STATIC
//#include <GL/glew.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

struct GLFWwindow;

class Window
{
public:
	Window(char* windowName, int w, int h);
	virtual ~Window();
public:
	bool isShouldClose();
	void swapBuffers();
	void setShouldClose(bool);
	void showWindow();

private:
	GLFWwindow* m_window = nullptr;
	char* m_window_name;
	int width = 0, height = 0;
};

#endif
