#include <Editor.h>


#include <thread>

Editor::Editor(Window* wind) : window(wind), ui(wind->getWindow(), &state), rendol(&currScene, &state, wind), selector(wind->getWidth(), wind->getHeight())
{
    GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
    GameState::instance = &state;
    GameState::editor_mode = 3;

    //TODO(darius) make it less ugly
    editorCamera.getCameraPosRef() = glm::vec3{0,0,4};//GameState::cam;
    GameState::editorCamera = &editorCamera;

    lastTime = glfwGetTime();

    currScene.start_scripts();

    SystemInfo::setInfo(&info);

    std::thread tr(&Editor::consoleInputThread, this, this);
    tr.detach();
}

void Editor::update()
{
    if (!lockFPS()) { return; }

    updateInput();
    printFPS();
    updateCamera();
    currScene.updateScene();

    rendol.render(window);

    if(showUI)
        ui.renderUI(currScene, rendol);

    rendol.updateBuffers(window);
}

void Editor::setEditorMode(int mode)
{
    GameState::editor_mode = mode;
}

void Editor::updateInput() {
    if(GameState::editorCameraMode && GameState::instance->ks.get_mouse_right_button()){
        if (GameState::instance->ks.get_w()) {
            if(GameState::editor_mode == 3)
                GameState::cam.moveCameraForward();
            else
                GameState::cam.moveCameraUp();
        }
        if (GameState::instance->ks.get_a()) {
            GameState::cam.moveCameraLeft();
        }
        if (GameState::instance->ks.get_s()) {
            if(GameState::editor_mode == 3)
                GameState::cam.moveCameraBackward();
            else
                GameState::cam.moveCameraDown();
        }
        if (GameState::instance->ks.get_d()) {
            GameState::cam.moveCameraRight();
        }
        if (GameState::instance->ks.get_q()) {
            GameState::cam.moveCameraBackward();
        }
        if (GameState::instance->ks.get_e()) {
            GameState::cam.moveCameraForward();
        }
    }
	if (GameState::instance->ks.get_c()) {
        //currScene.AddEmpty(currScene.getEmptyIndex());
    }
    if (GameState::instance->ks.get_0()) {
        debug_mode = true;
		//state.debug_msg.append("debug mode toogled\n");

        GameState::editorCameraMode = true;
		state.msg("debug mode toogled\n");
    }
    if (GameState::instance->ks.get_1()) {
        setEditorMode(2);
		state.debug_msg.append("2D\n");
    }
    if (GameState::instance->ks.get_2()) {
        setEditorMode(3);
		state.debug_msg.append("3D\n");
        state.connect--;
    }
    if (GameState::instance->ks.get_3()) {
        rendol.getDebugRenderer().debug_render= false;
        showUI = false;
    }
    if (GameState::instance->ks.get_4()) {
        rendol.getDebugRenderer().debug_render= true;
        showUI = true;
    }
    if (GameState::instance->ks.get_s() && GameState::instance->ks.get_cntrl()) {
        currScene.serialize(GameState::engine_path + "scene.dean");
    }
    if (GameState::instance->ks.get_l() && GameState::instance->ks.get_cntrl() ) {
        currScene.deserialize(GameState::engine_path + "scene.dean");
    }
    if (GameState::instance->ks.get_9()) {
        debug_mode = false;
        rendol.getDebugRenderer().clearPoints();
        GameState::editorCameraMode = false;
    }

    if(GameState::instance->ks.get_mouse_right_button() && GameState::editorCameraMode && !leftMouseButtonIsOnHold)
    {
        if (GameState::cam.cursor_hidden == false) {
            //GameState::ms.cursor_x = GameState::ms.get_x();
            //GameState::ms.cursor_y = GameState::ms.get_y();

            GameState::ms.set_x(GameState::ms.prev_x);
            GameState::ms.set_y(GameState::ms.prev_y);

            GameState::ms.set_firstX(GameState::ms.get_x());
            GameState::ms.set_firstY(GameState::ms.get_y());

            GameState::cam.cursor_hidden = true;
            glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window->getWindow(), GameState::ms.prev_x, GameState::ms.prev_y);
        }
        
        if (GameState::editor_mode == 2) {
            if (GameState::ms.prev_x > GameState::ms.get_x()) {
                GameState::cam.moveCameraLeft();
            }
            if (GameState::ms.prev_x < GameState::ms.get_x()) {
                GameState::cam.moveCameraRight();
            }
            if (GameState::ms.prev_y < GameState::ms.get_y()) {
                GameState::cam.moveCameraDown();
            }
            if (GameState::ms.prev_y > GameState::ms.get_y()) {
                GameState::cam.moveCameraUp();
            }
        }
        GameState::ms.prev_x = GameState::ms.get_x();
        GameState::ms.prev_y = GameState::ms.get_y();
    }

    if(!GameState::instance->ks.get_mouse_right_button())
    {
        if (GameState::cam.cursor_hidden == true) {
            //glfwSetCursorPos(window->getWindow(), GameState::ms.cursor_x, GameState::ms.cursor_y);

            //GameState::ms.set_x(GameState::ms.cursor_x);
            //GameState::ms.set_y(GameState::ms.cursor_y);

			//glfwSetCursorPos(window->getWindow(), GameState::ms.cursor_x, GameState::ms.cursor_y);
            glfwSetCursorPos(window->getWindow(), GameState::ms.get_firstX(), GameState::ms.get_firstY());
            glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


            GameState::cam.cursor_hidden = false;
        }
		GameState::ms.cursor_x = GameState::ms.get_x();
		GameState::ms.cursor_y = GameState::ms.get_y();
              
        //GameState::ms.set_x(GameState::ms.prev_x);
        //GameState::ms.set_y(GameState::ms.prev_y);
    }
    //std::cout << GameState::ms.get_x() << " " << GameState::ms.get_y()<< "\n";
    if(GameState::instance->ks.get_mouse_left_button() && !leftMouseButtonIsOnHold)
    {
        //selector.ProbeSceneObjects(&currScene, static_cast<float>(GameState::ms.click_x), static_cast<float>(GameState::ms.click_y), getWindow(), getRenderer());
        int picked = selector.ReadPixel(GameState::ms.click_x, getWindow()->getHeight() - 1 - GameState::ms.click_y);
        Object* pickedObj = currScene.getObjectByID(picked);
        if (pickedObj != nullptr) {
            ui.setItemClicked(pickedObj);
        }
        leftMouseButtonIsOnHold = true;
    }

    if(!GameState::instance->ks.get_mouse_left_button())
    {
        leftMouseButtonIsOnHold = false;
    }
}

void Editor::updateCamera()
{
    if (GameState::cam.cursor_hidden == false) {
        return;
    }

    if(GameState::editorCameraMode)
    {
        GameState::cam.setCameraLook(GameState::ms.prev_x, GameState::ms.prev_y);
        GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
    }

    //GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
}

void Editor::setPolygonMode(size_t type) {
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<unsigned int>(type));
}

void Editor::printFPS() {
    double currentTime = glfwGetTime();
    frame_number++;
    if (currentTime - lastTime >= 1.0) {
        //std::cout << "framerate: " << 1000.0 / double(frame_number) << "\n";
        frame_number = 0;
        lastTime += 1.0;
    }
}

bool Editor::lockFPS() {
    double time = glfwGetTime();
    double deltaTime = time - lastTime;
    if (deltaTime >= 1.0 / numOfFrames) {
        lastTime = time;
        return 1;
    }
    return 0;
}

Window* Editor::getWindow()
{
    return window;
}

Renderer* Editor::getRenderer()
{
    return &rendol;
}

void Editor::consoleInputThread(Editor* currEditor)
{
    //TODO(darius) make it separated object and not call methods directly, but pass message to main thread (this is detached thread btw)
    //DANGER(darius) NO SYNCHRONIZATION. CAN HAPPPEN ANYTHING 
    try {
        while (1)
        {
            std::string command;
            std::cout << "Input: \n";
            std::cin >> command;

            if (command == "createObject")
            {
                std::string objectName;
                std::cout << "ObjectName: \n";
                std::cin >> objectName;

                currEditor->currScene.AddObject(objectName);
            }

            if (command == "loadPrefab")
            {
                std::string prefabPath;
                std::cout << "prfabPath: ";

                std::cin >> prefabPath;
                //NOTE(darius) crashes 
                if (currEditor)
                    currEditor->currScene.deserialize(prefabPath);
            }

            if (command == "rotateUp")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                Object* obj = currEditor->currScene.getObjectByName(objName);
                float _90degree_in_radians = 1.57f;
                obj->getTransform().rotate(_90degree_in_radians, { 1,0,0 });
            }

            if (command == "rotate") {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                std::string axis;
                std::cout << "axis: ";

                std::cin >> axis;

                glm::vec3 axisVec = { 0,0,0 };

                if (axis == "x")
                    axisVec = { 1,0,0 };
                if (axis == "y")
                    axisVec = { 0,1,0 };
                if (axis == "z")
                    axisVec = { 0,0,1 };

                std::string degree;
                std::cout << "degree: ";

                float degreeFloat = 0.0f;

                std::cin >> degreeFloat;

                degreeFloat = glm::radians(degreeFloat);

                Object* obj = currEditor->currScene.getObjectByName(objName);
                obj->getTransform().rotate(degreeFloat, axisVec);
            }

            if (command == "hide")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                Object* obj = currEditor->currScene.getObjectByName(objName);
                obj->hide();
            }
            //NOTE(darius) crashes
            if (command == "copy")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                Object* obj = currEditor->currScene.getObjectByName(objName);
                Object tmp = *obj;

                currEditor->currScene.createObject(std::move(tmp));
            }

            if (command == "list")
            {
                for (Object* obj : currEditor->currScene.get_objects())
                {
                    std::cout << obj->get_name() << "\n";
                }
            }
        }
    }
    catch (...) 
    {
        std::terminate();
    }
}
