#pragma once

#include <UI.h>

class Editor
{
public:
	Editor(Window* wind);

	void update();

    void updateInput();

    void updateCamera();

    void setPolygonMode(size_t type = GL_LINE);

    void printFPS();
    
private:
    UI ui;
    Scene currScene;
    Window* window;
    Renderer rendol;

    unsigned int frame_number = 0;
    double lastTime = 0;

    bool debug_mode = false;
};
