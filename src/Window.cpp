#include "Window.h"
#include "Shader.h"
#include "InputManager.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include <stb/stb_image.h>

Window::Window(char* windowName, int w, int h)
{
	width = w;
	height = h;
	m_window_name = windowName;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_window = glfwCreateWindow(w, h, windowName, nullptr, nullptr);
	
	if(m_window == nullptr)
	{
		std::cerr << "cant create window\n";
		glfwTerminate(); 
		return;
	}

	glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, InputManager::framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw std::exception("can not init glad");
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

void Window::showWindow()
{
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec4 ourColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = ourColor;\n"
        "}\n\0";

    glfwMakeContextCurrent(m_window);

    Shader sv(vertexShaderSource, GL_VERTEX_SHADER);
    sv.compile();
    Shader sf(fragmentShaderSource, GL_FRAGMENT_SHADER);
    sf.compile();
    
    unsigned int shaderProgram = sv.link(sf);
    
    float vertices[] = {
         0.1f,  0.5f, 0.0f,
         0.8f, -0.5f, 0.0f,
        -0.9f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3 
    };

    VBO<12> vbo;        
    VAO vao;    
    EBO<sizeof(indices)> ebo;
    vao.bind();

    vbo.bind(vertices);
    vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    ebo.bind(indices);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    while (!glfwWindowShouldClose(m_window)) {
        InputManager::processInput(m_window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        sf.changeProgramUniformState(shaderProgram);
        glUseProgram(shaderProgram);
        
        vao.bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Window::swapBuffers() {
	glfwSwapBuffers(m_window);
}
