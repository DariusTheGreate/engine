#include <Editor.h>

Editor::Editor(Window* wind) : window(wind), ui(wind->getWindow(), &state), rendol(&currScene, &state, wind)
{
    GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
    GameState::instance = &state;
    GameState::editor_mode = 3;
    lastTime = glfwGetTime();
    currScene.start_scripts();
}

void Editor::update()
{
    if (!lockFPS()) { return; }
    updateInput();
    printFPS();
    updateCamera();
    currScene.updateScene();

    rendol.render(window);
    ui.renderUI(currScene, rendol);
    rendol.updateBuffers(window);
}

void Editor::setEditorMode(int mode)
{
    GameState::editor_mode = mode;
}

void Editor::updateInput() {
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
    if (GameState::instance->ks.get_0()) {
        debug_mode = true;
		//state.debug_msg.append("debug mode toogled\n");
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
        rendol.getDebugRenderer().debug_render_points = false;
        currScene.serialize("E:/own/programming/engine/scene.dean");
    }
    if (GameState::instance->ks.get_4()) {
        rendol.getDebugRenderer().debug_render_points = true;
        currScene.deserialize("E:/own/programming/engine/scene.dean");
    }
    if (GameState::instance->ks.get_9()) {
        debug_mode = false;
        rendol.getDebugRenderer().clearPoints();
    }
    if(GameState::instance->ks.get_mouse_right_button())
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

        GameState::ms.prev_x = GameState::ms.get_x();
        GameState::ms.prev_y = GameState::ms.get_y();

        //glfwSetCursorPos(window->getWindow(), GameState::ms.prev_x, GameState::ms.prev_y);
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
    if(GameState::instance->ks.get_mouse_left_button())
    {
        selector.ProbeSceneObjects(&currScene, static_cast<float>(GameState::ms.click_x), static_cast<float>(GameState::ms.click_y), getWindow(), getRenderer());
        selector.ReadPixel(GameState::ms.click_x, getWindow()->getHeight() - 1 - GameState::ms.click_y);
    }
}

void Editor::updateCamera()
{
    if (GameState::cam.cursor_hidden == false) {
        return;
    }
    GameState::cam.setCameraLook(GameState::ms.prev_x, GameState::ms.prev_y);
    //GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
    GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
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

