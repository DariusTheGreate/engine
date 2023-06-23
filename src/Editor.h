#pragma once

#include <UI.h>
#include <ObjectSelector.h>

class Editor
{
public:
	Editor(Window* wind);

	void update();

    void updateInput();

    void updateCamera();

    void setPolygonMode(size_t type = GL_LINE);

    void printFPS();

    Window* getWindow();
    
private:
    UI ui;
    Scene currScene;
    Window* window;
    Renderer rendol;
    ObjectSelector selector;

    unsigned int frame_number = 0;
    double lastTime = 0;

    bool debug_mode = false;
};
