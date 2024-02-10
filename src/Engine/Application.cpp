#define WIN32_LEAN_AND_MEAN//NOTE(darius) to avoid conflicts with boost::asio
#define _CRT_SECURE_NO_WARNINGS 
#include <windows.h>

#include <Engine/Application.h>
#include <Rendering/Renderer.h>
#include <Engine/Editor.h>


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

