#include "Application.h"
#include "Window.h"

Application::Application(char* name, int windowCount, int windowW, int windowH) : m_app_name(name), m_running(false), m_windows_count(windowCount)
{
	m_windows = new Window*[windowCount];
	for(int i = 0; i < windowCount; ++i)
	{
		m_windows[i] = new Window(name, windowW, windowH);
	}
}

Application::~Application() 
{
	delete[] m_windows;
}

void Application::Run()
{
	
	m_running = true;
}