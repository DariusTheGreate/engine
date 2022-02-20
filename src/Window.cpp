#include "Window.h"
#include "Shader.h"
#include "InputManager.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Texture.h"

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    glfwSetFramebufferSizeCallback(m_window, InputManager::framebuffer_size_standart_callback);

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
    glfwMakeContextCurrent(m_window);

    Shader sv("shaders/vertexShader.glsl", GL_VERTEX_SHADER);
    sv.compile();

    Shader sf("shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);
    sf.compile();
    
    unsigned int shaderProgram = sv.link(sf);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  -6.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3 
    };

    VBO<sizeof(vertices)> vbo;
    VAO vao;
    EBO<sizeof(indices)> ebo;
    vao.bind();

    vbo.bind(vertices);
    vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vbo.vboEnableVertexAttribArray(0);

    //vbo.setVAO(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //vbo.vboEnableVertexAttribArray(1);

    vbo.setVAO(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    vbo.vboEnableVertexAttribArray(2);

    stbi_set_flip_vertically_on_load(true);
    ebo.bind(indices);
    Texture t1("putin.jpg", GL_RGB, GL_RGB);

    //glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!isShouldClose()) {
        InputManager::standart_input_processor(m_window);
        t1.activate(GL_TEXTURE1);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        sv.setMat4("projection", projection, shaderProgram); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        sv.setMat4("view", view, shaderProgram);

        // render boxes
        vao.bind();
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            sv.setMat4("model", model, shaderProgram);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Window::swapBuffers() {
	glfwSwapBuffers(m_window);
}
