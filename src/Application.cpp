#include "Application.h"
#include "Window.h"

#include "Renderer.h"
#include <Editor.h>

Application::Application(char* name, int windowW, int windowH) : m_app_name(name), m_window(new Window(name, windowW, windowH)) {}

Application::~Application() 
{
	delete m_window;
}

void Application::Run()
{
	Editor edik(m_window);

    while (!m_window->isShouldClose()) {
        InputManager::processInput(m_window->getWindow());
        edik.update();
    }
}

