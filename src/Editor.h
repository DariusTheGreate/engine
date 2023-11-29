#pragma once

#include <Server.h>
#include <Client.h>

#include <UI.h>
#include <ObjectSelector.h>
#include <SystemInfo.h>

#include <memory>
#include <queue>
#include <functional>

enum class Event
{
    LoadScene,
    CreateObject,
    ManipulateObject,
    NoEvent,
};

class EditorEvent
{
public:
    EditorEvent() = default;

    EditorEvent(Event ev_in) : ev(ev_in) {}

    Event getEventType()
    {
        return ev;
    }

private:
    Event ev = Event::NoEvent;
};

class LoadSceneEvent : public EditorEvent
{
public:
    LoadSceneEvent() = default;

    LoadSceneEvent(const std::string& str) : EditorEvent(Event::LoadScene), scene(str) {}

    std::string getScene()
    {
        return scene;
    }

private:
    std::string scene;
};

class CreateObjectEvent : public EditorEvent
{
public:
    CreateObjectEvent() = default;

    CreateObjectEvent(const std::string& str) : EditorEvent(Event::CreateObject), name(str) {}

    std::string getName()
    {
        return name;
    }

private:
    std::string name;
};

class ManipulateObjectEvent : public EditorEvent
{
public:
    ManipulateObjectEvent() = default;

    ManipulateObjectEvent(Object* obj_in, std::function<void(Object*)> f) : EditorEvent(Event::ManipulateObject), obj(obj_in), functor(f) {}

    void operator ()()
    {
        functor(obj);
    }

private:
    Object* obj;
    std::function<void(Object*)> functor;
};


class Editor
{
public:
	Editor(Window* wind);

	void update();

    void updateInput();

    void updateCamera();

    void processEvents();

    //TODO(darius) make it constexpr dispath
    template<typename... Args>
    void addEvent(Event e, Args&&... args)
    {   
        EditorEvent* ev = nullptr;

        if (e == Event::LoadScene) 
            ev = new LoadSceneEvent(std::forward<Args>(args)...);
        else if (e == Event::CreateObject)
            ev = new CreateObjectEvent(std::forward<Args>(args)...);
        //else if (e == Event::ManipulateObject)
        //    ev = new ManipulateObjectEvent(std::forward<Args>(args)...);
        else
            ev = new EditorEvent();

        events.push(ev);
    }

    template<typename... Args>
    void addManipualtionEvent(Event e, Args&&... args)
    {   
        EditorEvent* ev = nullptr;

        ev = new ManipulateObjectEvent(std::forward<Args>(args)...);

        events.push(ev);
    }

    static void addProfiler(std::string&& name);

    static void addProfileRecord(std::string&& name, float record);

    void setPolygonMode(size_t type = GL_LINE);

    void setEditorMode(int mode);

    void printFPS();

    bool lockFPS();

    void consoleInputThread(Editor* currentEditor);

    Window* getWindow();

    Renderer* getRenderer();

    static Scene* getScene();

    static void fileDropCallbackDispatch(std::string_view str);

    void createServer();
    std::shared_ptr<Server> getServer();

    void createClient();
    std::shared_ptr<Client> getClient();

public:
    bool consoleOnly = false;
    
    static Window* window;
    
private:
    GameState state;
    Camera editorCamera;
    SystemInfo info;
    UI ui;
    
    static Scene currScene;

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

    std::queue<EditorEvent*> events;

    static std::vector<Profiler<float>> profilers;//TODO(darius) update somehow and give to UI to render
};
