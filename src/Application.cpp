#include "Application.h"
#include "Window.h"

Application::Application(char* name, int windowCount, int windowW, int windowH) : m_app_name(name), m_running(false), m_windows_count(windowCount), m_curr_window(0)
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
	m_running = false;
}

void Application::Run()
{
	m_running = true;
	m_windows[m_curr_window]->showWindow();
}

void Application::setCurrWindow(int i)
{
	m_curr_window = i;
}
