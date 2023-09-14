#include <Editor.h>
#include <Timer.h>

#include <iostream>
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

    //TODO(darius) move it to texture or something
    GLFWimage images[1]; 
    images[0].pixels = stbi_load(std::string(GameState::engine_path + "assets/logo.png").c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(wind->getWindow(), 1, images); 
    stbi_image_free(images[0].pixels); 

    //TODO(darius) DANGER
    std::thread tr(&Editor::consoleInputThread, this, this);
    tr.detach();
}

void Editor::update()
{
    if (!lockFPS()) { return; }

    Timer t;
    t.setBoolPrint(false);
    
    updateInput();
    printFPS();
    updateCamera();
    currScene.updateScene();
    processEvents();

    if(consoleOnly)
        return;

    rendol.render(window);

    if(GameState::editor_mode != 0){
        ui.setTime(t.checkTime());
        ui.renderUI(currScene, rendol);
    }

    rendol.updateBuffers(window);
}

void Editor::setEditorMode(int mode)
{
    GameState::editor_mode = mode;
}

void Editor::updateInput() {
    if(GameState::editorCameraMode && GameState::instance->ks.get_mouse_right_button()){
        if (GameState::instance->ks.get_w()) {
            if(GameState::editor_mode == 3 || GameState::editor_mode == 0)
                GameState::cam.moveCameraForward();
            else
                GameState::cam.moveCameraUp();
        }
        if (GameState::instance->ks.get_a()) {
            GameState::cam.moveCameraLeft();
        }
        if (GameState::instance->ks.get_s()) {
            if(GameState::editor_mode == 3 || GameState::editor_mode == 0)
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
	if (GameState::instance->ks.get_c() && GameState::instance->ks.get_cntrl()) {
        Object tmp = *ui.getItemClicked();
        currScene.createObject(tmp);
        //currScene.AddEmpty(currScene.getEmptyIndex());
    }
    if (GameState::instance->ks.get_d() && GameState::instance->ks.get_cntrl()) {
        Object* item_clicked = ui.getItemClicked();
        if(item_clicked){
            currScene.destroyObject(item_clicked->get_name());
            ui.setItemClickedForced(nullptr);
        }
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
        //showUI = false;
        currScene.batchProbeSimilarObjects();
        GameState::editor_mode = 0;
    }
    if (GameState::instance->ks.get_4()) {
        rendol.getDebugRenderer().debug_render= true;
        //showUI = true;
        currScene.recoverBatchedObjects();
        GameState::editor_mode = 3;
    }
    if (GameState::instance->ks.get_s() && GameState::instance->ks.get_cntrl()) {
        currScene.serialize(GameState::engine_path + "scene.dean");
    }
    if (GameState::instance->ks.get_l() && GameState::instance->ks.get_cntrl() ) {
        currScene.deserialize(GameState::engine_path + "scene.dean");
    }
    if(GameState::instance->ks.get_lshift() && !leftShiftIsOnHold){
        *GameState::cam.getCameraSpeed() *= speedMultiplyFactor;
        leftShiftIsOnHold = true;
    }
    if(!GameState::instance->ks.get_lshift() && leftShiftIsOnHold){
        *GameState::cam.getCameraSpeed() /= speedMultiplyFactor;
        leftShiftIsOnHold = false;
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
            /*if(currScene.recoverBatch(pickedObj))
            {
                rendol.EditorIDsStage();
                picked = selector.ReadPixel(GameState::ms.click_x, getWindow()->getHeight() - 1 - GameState::ms.click_y);
                pickedObj = currScene.getObjectByID(picked);
                if(pickedObj)
                    ui.setItemClicked(pickedObj);
            }
            */
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

//TODO(darius) work on event system. reinterpret cast is cringe, but only other way is to use virtual functions and i dont know whats worse, probably reinrpret cast but leave it as it is for now
void Editor::processEvents()
{
    while(!events.empty())
    {
        std::cout << "event processing\n";
        auto* e = events.front();
        events.pop();

        if(e->getEventType() == Event::LoadScene)
        {
            auto str = reinterpret_cast<LoadSceneEvent*>(e)->getScene();
            currScene.parseScene(str);
            delete reinterpret_cast<LoadSceneEvent*>(e);
        }
        else if(e->getEventType() == Event::CreateObject)
        {
            auto name = reinterpret_cast<CreateObjectEvent*>(e)->getName();
            currScene.AddObject(name);
            delete reinterpret_cast<CreateObjectEvent*>(e);
        }
        else if(e->getEventType() == Event::ManipulateObject)
        {
            auto* func = reinterpret_cast<ManipulateObjectEvent*>(e);
            func->operator()();

            delete reinterpret_cast<ManipulateObjectEvent*>(e);
        }
        else
        {
            delete reinterpret_cast<EditorEvent*>(e);
        }
    }
}

void Editor::setPolygonMode(size_t type) {
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<unsigned int>(type));
}

//TODO(darius) switch to Timer here
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


Scene* Editor::getScene()
{
    return &Editor::currScene;
}

void Editor::fileDropCallbackDispatch(std::string_view path)
{
    //NOTE(darius)this part is called only if callback initialized. So theres no problem, but mb nake it better somehow 
    //TODO(darius) make some file openeer system to get rid of copypast ehre and in UI

    std::cout << path << "\n";

    if(path.find(".PREFAB") != std::string_view::npos | path.find(".prefab") != std::string_view::npos)
    {
        currScene.deserialize(path);
    }
    else if(path.find(".DEAN") != std::string_view::npos || path.find(".dean") != std::string_view::npos)
    {
        currScene.deserialize(path);
    }
    else if(path.find(".png") != std::string_view::npos)
    {
        FlatMesh* flat = currScene.createFlatMesh();
        flat->setTexture(std::string(path));

        Object* obj = currScene.AddEmpty(666);

        LightingShaderRoutine shaderRoutine;
        Shader currShader = Renderer::shaderLibInstance->getCurrShader();

        obj->addModel(std::move(*flat), currShader, shaderRoutine);
    }
    else if(path.find(".obj") != std::string_view::npos)
    {
        Shader currShader = Renderer::shaderLibInstance->getCurrShader();
        LightingShaderRoutine shaderRoutine;

        Object* obj = currScene.AddEmpty(666);
        obj->addModel(path, currShader, shaderRoutine);
    }
    else {
        std::cout << "fuk u wtf\n";
    }
}

void Editor::createServer()
{
    if(server || client) 
        return;
    server = std::make_shared<Server>(currScene.getNetworkSynchronizer(), currScene);
}

std::shared_ptr<Server> Editor::getServer()
{
    return server; 
}

void Editor::createClient()
{
    if(client || server) 
        return;
    client = std::make_shared<Client>();
}

std::shared_ptr<Client> Editor::getClient()
{
    return client; 
}

void Editor::consoleInputThread(Editor* currEditor)
{
    if (!currEditor)
        return;
    
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

                currEditor->addEvent(Event::CreateObject, std::move(objectName));

                //currEditor->currScene.AddObject(objectName);
            }

            if (command == "loadPrefab")
            {
                std::string prefabPath;
                std::cout << "prfabPath: ";

                std::cin >> prefabPath;
                //NOTE(darius) crashes 
				currEditor->currScene.deserialize(prefabPath);
            }

            if (command == "rotateUp")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                Object* obj = currEditor->currScene.getObjectByName(objName);

                currEditor->addManipualtionEvent(Event::ManipulateObject, obj, [](Object* obj){
                    float _90degree_in_radians = 1.57f;
                    obj->getTransform().rotate(_90degree_in_radians, { 1,0,0 });
                });
                
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

            if (command == "listBatches")
            {
                auto cache = currEditor->currScene.getBatchCache().takeBack();

                for (Object* obj : cache)
                {
                    std::cout << obj->get_name() << "\n";
                }
            }

            if(command == "printMesh")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                Object* obj = currEditor->currScene.getObjectByName(objName);

                if(obj && obj->getModel()){
                    obj->getModel()->meshes[0].printVertices();
                }
                else{
                    std::cout << "smthng wrong\n";
                }
            }

            if(command == "batchMesh")
            {
                std::string objName;
                std::cout << "objName: ";
                std::cin >> objName;

                std::string objName2;
                std::cout << "objName2: ";
                std::cin >> objName2;

                Object* obj = currEditor->currScene.getObjectByName(objName);
                Object* obj2 = currEditor->currScene.getObjectByName(objName2);

                if(obj && obj2 && obj->getModel() && obj2->getModel()){
                    /*obj->getModel()->meshes[0].addVerticesBath(obj2->getModel()->meshes[0]);

                    Mesh* mOrigin = &obj->getModel()->meshes[0];

                    auto inds = mOrigin->getIndices(); 
                    auto verts = mOrigin->getVertices(); 
                    auto texts = mOrigin->getTextures(); 

                    auto* mBatched = currEditor->currScene.createMesh(std::move(verts), std::move(inds), std::move(texts));

                    Model m;
                    m.addMesh(*mBatched);

                    auto* objBatched = currEditor->currScene.AddObject("batched");

                    objBatched->addModel();
                    objBatched->getModel()->addMesh(*mBatched);
                    */
                }
            }

            if(command == "server")
            {
                currEditor->createServer();
                auto editorServer = currEditor->getServer();
                editorServer->listen();
            }

            if(command == "client")
            {
                std::string port;
                std::cout << "Port: ";
                std::cin >> port;
                currEditor->createClient();
                auto client = currEditor->getClient();
                client->serverRegistration();

                while(1){
                    std::string msg;
                    std::cout << "Msg: ";
                    std::cin >> msg;
                    auto res = client->query(port, msg);
                    //std::cout << res;
                    if(msg == "scene@")
                    {
                        //currEditor->getScene()->parseScene(std::move(res)); 
                        currEditor->addEvent(Event::LoadScene, std::move(res));
                    }
                }

                //client->query(currEditor->currScene.readFileToString(GameState::engine_path + "scene.dean"));
            }

            if(command == "clientSync")
            {
                std::string port;
                std::cout << "Port: ";
                std::cin >> port;
                currEditor->createClient();
                auto client = currEditor->getClient();
                client->serverRegistration();

                auto& syncer = currEditor->getScene()->getNetworkSynchronizer();

                while(1)
                {
                    client->sync(port, syncer);
                }
            }

            if(command == "nogui")
            {
                currEditor->consoleOnly = true;
            }

            if(command == "gui")
            {
                currEditor->consoleOnly = false;
            }

            //TODO(darius) lagging for some reason
            if(command == "netSync")
            {
                std::string name;
                std::cout << "Name: ";
                std::cin >> name;

                Object* obj = currEditor->currScene.getObjectByName(name);

                if(obj)
                    currEditor->currScene.addObjectToNetSync(obj);
                else{
                    std::cout << "Couldnt find object " << name << "\n";
                }
            }
        }
    }
    catch (...) 
    {
        std::terminate();
    }
}

Scene Editor::currScene;
