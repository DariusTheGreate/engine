#pragma once

#include <UI.h>
#include <ObjectSelector.h>
#include <SystemInfo.h>

class Editor
{
public:
	Editor(Window* wind);

	void update();

    void updateInput();

    void updateCamera();

    void setPolygonMode(size_t type = GL_LINE);

    void setEditorMode(int mode);

    void printFPS();

    bool lockFPS();

    void consoleInputThread(Editor* currentEditor);

    Window* getWindow();

    Renderer* getRenderer();

private:
    GameState state;
    Camera editorCamera;
    SystemInfo info;
    UI ui;
    Scene currScene;
    Window* window;
    Renderer rendol;
    ObjectSelector selector;

    unsigned int frame_number = 0;
    double lastTime = 0;
    double numOfFrames = 60;

    bool debug_mode = false;
    bool showUI = true;

    //NOTE(darius) is this cringe?
    bool leftMouseButtonIsOnHold = false;//NOTE(darius) its here in order to not select object when u already dragging some other object over it
    bool leftShiftIsOnHold = false;

    float speedMultiplyFactor = 3;
};
