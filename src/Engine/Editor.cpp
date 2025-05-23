#define _WIN32_WINNT 0x0601 

#include <Engine/Editor.h>
#include <Core/Timer.h>
#include <Core/Printer.h>

#include <iostream>
#include <thread>
#include <cmath>

//NOTE(darius) no UB here.
void EditorEvent::operator delete(EditorEvent* p, std::destroying_delete_t) {
    switch (p->ev) {
        case Event::LoadScene:
            static_cast<LoadSceneEvent*>(p)->~LoadSceneEvent();
            break;
        case Event::CreateObject:
            static_cast<CreateObjectEvent*>(p)->~CreateObjectEvent();
            break;
        case Event::ManipulateObject:
            static_cast<ManipulateObjectEvent*>(p)->~ManipulateObjectEvent();
            break;
    }
    ::operator delete(p);
}

Editor::Editor(Window* wind) : ui(wind->getWindow(), &state), rendol(&currScene, &state, wind), selector(wind->getWidth(), wind->getHeight())
{
    Editor::window = wind;
    GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
    GameState::instance = &state;
    GameState::editor_mode = 3;

    //TODO(darius) make it less ugly
    editorCamera.getCameraPosRef() = glm::vec3{0,0,4};//GameState::cam;
    GameState::editorCamera = &editorCamera;

    lastTime = glfwGetTime();

    currScene.start_scripts();

    SystemInfo::setInfo(&info);

    ImageUtils::ImageLoaderPtr imageLoader(GameState::engine_path + "assets/logo.png");
    GLFWimage images[1]; 
    unsigned char rawData[] = {255, 0, 0, 255, 0, 0, 255, 0, 0, 
                             0, 255, 0, 0, 255, 0, 0, 255, 0,
                             0, 0, 255, 0, 0, 255, 0, 0, 255};
    //NOTE(darius) can use rawData/3/3 here:
    images[0].pixels = imageLoader.data;
    images[0].width = imageLoader.W;
    images[0].height = imageLoader.H;
    glfwSetWindowIcon(wind->getWindow(), 1, images); 

    //TODO(darius) DANGER
    std::thread tr(&Editor::consoleInputThread, this, this);
    tr.detach();

    Renderer::currentRendererPtr = &rendol;
    
    CameraPoints points;
    points.points = { {0.0f, {0.0f, 0.0f, 0.0f}}, {3.0f, {1.0f, 1.0f, 1.0f}}, {6.0f, {2.0f, 2.0f, 2.0f}},
    {20.0f, {4.0f, 4.0f, 4.0f}}, {60.0f, {8.0f, 8.0f, 8.0f}}};

    points.currPoint = 0;
    //GameState::setCameraMotionPoints(points);
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
        ui.setTime(t.checkTime());//TODO: move it to gamestate
        ui.renderUI(currScene, rendol);
    }

    rendol.updateBuffers(window);

    //GameState::currTime += t.checkTime();
}

void Editor::setEditorMode(int mode)
{
    GameState::editor_mode = mode;
}

void Editor::updateInput() {
    if(GameState::editorCameraMode && GameState::instance->ks.get_mouse_right_button()){
        if (GameState::instance->ks.get_w()) {
            if(GameState::editor_mode == 3 || GameState::editor_mode == 0)
                GameState::instance->cam.moveCameraForward();
            else
                GameState::instance->cam.moveCameraUp();
        }
        if (GameState::instance->ks.get_a()) {
            GameState::instance->cam.moveCameraLeft();
        }
        if (GameState::instance->ks.get_s()) {
            if(GameState::editor_mode == 3 || GameState::editor_mode == 0)
                GameState::instance->cam.moveCameraBackward();
            else
                GameState::instance->cam.moveCameraDown();
        }
        if (GameState::instance->ks.get_d()) {
            GameState::instance->cam.moveCameraRight();
        }
        if (GameState::instance->ks.get_q()) {
            GameState::instance->cam.moveCameraBackward();
        }
        if (GameState::instance->ks.get_e()) {
            GameState::instance->cam.moveCameraForward();
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
            currScene.destroyObject(item_clicked->getName());
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
    if (GameState::instance->ks.get_1() && GameState::instance->ks.get_cntrl()) {
        setEditorMode(2);
		state.debug_msg.append("2D\n");
    }
    if (GameState::instance->ks.get_2() && GameState::instance->ks.get_cntrl()) {
        setEditorMode(3);
		state.debug_msg.append("3D\n");
    }
    if (GameState::instance->ks.get_3() && GameState::instance->ks.get_cntrl()) {
        rendol.getDebugRenderer().debug_render= false;
        //showUI = false;
        //currScene.batchProbeSimilarObjects();
        GameState::editor_mode = 0;
    }
    if (GameState::instance->ks.get_4() && GameState::instance->ks.get_cntrl()) {
        rendol.getDebugRenderer().debug_render= true;
        //showUI = true;
        //currScene.recoverBatchedObjects();
        GameState::editor_mode = 3;
    }
    if (GameState::instance->ks.get_s() && GameState::instance->ks.get_cntrl()) {
        currScene.serialize(GameState::engine_path + "scene.dean");
    }
    if (GameState::instance->ks.get_l() && GameState::instance->ks.get_cntrl() ) {
        currScene.deserialize(GameState::engine_path + "scene.dean");
    }
    if(GameState::instance->ks.get_lshift() && !leftShiftIsOnHold){
        *GameState::instance->cam.getCameraSpeed() *= speedMultiplyFactor;
        leftShiftIsOnHold = true;
    }
    if(!GameState::instance->ks.get_lshift() && leftShiftIsOnHold){
        *GameState::instance->cam.getCameraSpeed() /= speedMultiplyFactor;
        leftShiftIsOnHold = false;
    }
    if (GameState::instance->ks.get_9()) {
        debug_mode = false;
        rendol.getDebugRenderer().clearPoints();
        GameState::editorCameraMode = false;
    }

    if(GameState::instance->ks.get_mouse_right_button() && GameState::editorCameraMode && !leftMouseButtonIsOnHold)
    {
        if (GameState::instance->cam.cursor_hidden == false) {
            //GameState::ms.cursor_x = GameState::ms.get_x();
            //GameState::ms.cursor_y = GameState::ms.get_y();

            GameState::ms.set_x(GameState::ms.prev_x);
            GameState::ms.set_y(GameState::ms.prev_y);

            GameState::ms.set_firstX(GameState::ms.get_x());
            GameState::ms.set_firstY(GameState::ms.get_y());

            GameState::instance->cam.cursor_hidden = true;
            glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(window->getWindow(), GameState::ms.prev_x, GameState::ms.prev_y);
        }
        
        if (GameState::editor_mode == 2) {
            if (GameState::ms.prev_x > GameState::ms.get_x()) {
                GameState::instance->cam.moveCameraRight();
            }
            if (GameState::ms.prev_x < GameState::ms.get_x()) {
                GameState::instance->cam.moveCameraLeft();
            }
            if (GameState::ms.prev_y < GameState::ms.get_y()) {
                GameState::instance->cam.moveCameraUp();
            }
            if (GameState::ms.prev_y > GameState::ms.get_y()) {
                GameState::instance->cam.moveCameraDown();
            }
        }
        GameState::ms.prev_x = GameState::ms.get_x();
        GameState::ms.prev_y = GameState::ms.get_y();
    }

    if(!GameState::instance->ks.get_mouse_right_button())
    {
        if (GameState::instance->cam.cursor_hidden == true) {
            //glfwSetCursorPos(window->getWindow(), GameState::ms.cursor_x, GameState::ms.cursor_y);

            //GameState::ms.set_x(GameState::ms.cursor_x);
            //GameState::ms.set_y(GameState::ms.cursor_y);

			//glfwSetCursorPos(window->getWindow(), GameState::ms.cursor_x, GameState::ms.cursor_y);
            glfwSetCursorPos(window->getWindow(), GameState::ms.get_firstX(), GameState::ms.get_firstY());
            glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


            GameState::instance->cam.cursor_hidden = false;
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
    if (GameState::instance->cam.cursor_hidden == false) {
		if (GameState::cameraMotionPoints.points.size() > 0)
		{
			auto& points = GameState::cameraMotionPoints.points;

			size_t idx = GameState::cameraMotionPoints.currPoint;
            size_t idx2 = idx + 1;

            if (idx2 >= points.size()) {
                GameState::cameraMotionPoints.currPoint = 0;
                //idx = 0;
                //idx2 = idx+1;
                
                GameState::instance->cam.setCameraPos(points.at(0).tr);

				println("currPos: ", GameState::cameraMotionPoints.currPoint, " position: ", points.at(0).tr);

                //std::vector<CameraPoints::Point> reversedPoints;
                //for (CameraPoints::Point* e = &points.back(); e >= &points.front(); e--)
                  //  reversedPoints.emplace_back(*e);
                //points = reversedPoints;
            }
            else if(GameState::currTime.checkTime() <= points.back().timeStamp) {
                double ttotal = 0.0f;
                if(idx > 0)
					ttotal = (GameState::currTime.checkTime() - points.at(idx-1).timeStamp) / (points.at(idx).timeStamp  - points.at(idx-1).timeStamp);

                double xl = std::lerp(points.at(idx).tr.x, points.at(idx2).tr.x, ttotal);
                double yl = std::lerp(points.at(idx).tr.y, points.at(idx2).tr.y, ttotal);
                double zl = std::lerp(points.at(idx).tr.z, points.at(idx2).tr.z, ttotal);

                glm::vec3 posl = { xl, yl, zl };
                println("currPos: ", GameState::cameraMotionPoints.currPoint, " position: ", posl);
                GameState::instance->cam.setCameraPos(posl);
                //rendol.getDebugRenderer().renderDebugPoint(posl, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});

                if (GameState::currTime.checkTime() >= points.at(idx).timeStamp)
                    GameState::cameraMotionPoints.currPoint = idx2;
            }
		}
        return;
    }

    if(GameState::editorCameraMode)
    {
        GameState::instance->cam.setCameraLook(GameState::ms.prev_x, GameState::ms.prev_y);
        GameState::instance->cam.setScrollState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
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
            //delete reinterpret_cast<LoadSceneEvent*>(e);
        }
        else if(e->getEventType() == Event::CreateObject)
        {
            auto name = reinterpret_cast<CreateObjectEvent*>(e)->getName();
            currScene.AddObject(name);
            //delete reinterpret_cast<CreateObjectEvent*>(e);
        }
        else if(e->getEventType() == Event::ManipulateObject)
        {
            auto* func = reinterpret_cast<ManipulateObjectEvent*>(e);
            func->operator()();

            //delete reinterpret_cast<ManipulateObjectEvent*>(e);
        }
        else
        {
            //delete reinterpret_cast<EditorEvent*>(e);
        }

        delete e;
    }
}

void Editor::addProfiler(std::string&& name)
{
    profilers.push_back(Profiler<float>(std::move(name)));
}

void Editor::addProfileRecord(std::string&& name, float record)
{
    for(auto& p : profilers)
    {
        if(p.getNameRef() == name)
            p.addRecord(std::move(record));
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

    if((path.find(".PREFAB") != std::string_view::npos) || (path.find(".prefab") != std::string_view::npos))
    {
        currScene.deserialize(path);
    }
    else if(path.find(".DEAN") != std::string_view::npos || path.find(".dean") != std::string_view::npos)
    {
        currScene.deserialize(path);
    }
    else if(path.find(".png") != std::string_view::npos || path.find(".jpg") != std::string_view::npos || path.find(".tga") != std::string_view::npos)
    {
        FlatMesh* flat = currScene.createFlatMesh();
        flat->setTexture(std::string(path));

        Object* obj = currScene.AddEmpty(666);

        Shader currShader = Renderer::shaderLibInstance->getCurrShader();

        obj->addModel(std::move(*flat));
    }
    else if(path.find(".obj") != std::string_view::npos)
    {
        Shader currShader = Renderer::shaderLibInstance->getCurrShader();

        Object* obj = currScene.AddEmpty(666);
        obj->addModel(path);
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
                if(obj)
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
                for (Object* obj : currEditor->currScene.getObjects())
                {
                    std::cout << obj->getName() << "\n";
                }
            }

            if (command == "listBatches")
            {
                auto cache = currEditor->currScene.getBatchCache().takeBack();

                for (Object* obj : cache)
                {
                    std::cout << obj->getName() << "\n";
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

            //NOTE(darius) to test multiplayer behaviour 1)netSync Object 2)server 3)netSync Object 4) clientSync 25001 5) test it
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
                    client->sync(port, syncer, currEditor->getScene());
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

            if(command == "test")
            {
                /*print("search object by name test...\n");
                Object* obj = currEditor->currScene.getObjectByName("UpAnim");
                if(!obj)
                    print("wont work\n");
                else
                    print("will work ", obj->get_name(), "\n");
                    */

                print("test camera move\n");
                GameState::instance->cam.moveCameraLeft();
            }
        }
    }
    catch (...) 
    {
        std::terminate();
    }
}

Scene Editor::currScene;

Window* Editor::window = nullptr;

std::vector<Profiler<float>> Editor::profilers;
