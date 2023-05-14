#include "Application.h"
#include "Window.h"

#include "Renderer.h"

Application::Application(char* name, int windowW, int windowH) : m_app_name(name), m_window(new Window(name, windowW, windowH)) {}

Application::~Application() 
{
	delete m_window;
}

void Application::Run()
{
    Renderer rendol(m_window);

    while (!m_window->isShouldClose()) {
        InputManager::processInput(m_window->getWindow());
        
        rendol.render(m_window);
    }
}

