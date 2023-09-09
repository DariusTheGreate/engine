#pragma once

#include <Server.h>
#include <Client.h>

#include <UI.h>
#include <ObjectSelector.h>
#include <SystemInfo.h>

#include <memory>

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

    static void fileDropCallbackDispatch(std::string_view str);

    void createServer();
    std::shared_ptr<Server> getServer();

    void createClient();
    std::shared_ptr<Client> getClient();

public:
    bool consoleOnly = false;

private:
    GameState state;
    Camera editorCamera;
    SystemInfo info;
    UI ui;
    static Scene currScene;
    Window* window;
    Renderer rendol;
    ObjectSelector selector;

    unsigned int frame_number = 0;
    double lastTime = 0;
    double numOfFrames = 60;

    bool debug_mode = false;


    //NOTE(darius) is this cringe?
    bool leftMouseButtonIsOnHold = false;//NOTE(darius) its here in order to not select object when u already dragging some other object over it
    bool leftShiftIsOnHold = false;

    float speedMultiplyFactor = 3;

    std::shared_ptr<Server> server;//NOTE(darius) tempo
    std::shared_ptr<Client> client;//NOTE(darius) tempo
};
