#include <Editor.h>

Editor::Editor(Window* wind) : window(wind), ui(wind->getWindow(), &state), rendol(&currScene, &state)
{
    GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
    GameState::editor_mode = 3;
    lastTime = glfwGetTime();
}

void Editor::update()
{
	updateInput();
	updateCamera();
    printFPS();

	currScene.updateScene();
    rendol.render(window, debug_mode);

	ui.renderUI(currScene, rendol);
   	rendol.updateBuffers(window); 
}

void Editor::setEditorMode(int mode)
{
    GameState::editor_mode = mode;
}

void Editor::updateInput() {
    if (GameState::ks.get_w()) {
        if(GameState::editor_mode == 3)
            GameState::cam.moveCameraForward();
        else
            GameState::cam.moveCameraUp();
    }
    if (GameState::ks.get_a()) {
        GameState::cam.moveCameraLeft();
    }
    if (GameState::ks.get_s()) {
        if(GameState::editor_mode == 3)
            GameState::cam.moveCameraBackward();
        else
            GameState::cam.moveCameraDown();
    }
    if (GameState::ks.get_d()) {
        GameState::cam.moveCameraRight();
    }
    if (GameState::ks.get_q()) {
        GameState::cam.moveCameraBackward();
    }
    if (GameState::ks.get_e()) {
        GameState::cam.moveCameraForward();
    }
    if (GameState::ks.get_0()) {
        debug_mode = true;
		state.debug_msg.append("debug mode toogled\n");
    }
    if (GameState::ks.get_1()) {
        setEditorMode(2);
		state.debug_msg.append("2D\n");
    }
    if (GameState::ks.get_2()) {
        setEditorMode(3);
		state.debug_msg.append("3D\n");
        static_cast<FlatMesh*>(&currScene.get_object_at(2)->getModel()->meshes[0])->setTextureCoords(0.5,0.5,0,0);
    }
    if (GameState::ks.get_3()) {
        rendol.getDebugRenderer().debug_render_points = false;
    }
    if (GameState::ks.get_4()) {
        rendol.getDebugRenderer().debug_render_points = true;
    }
    if (GameState::ks.get_9()) {
        debug_mode = false;
        rendol.getDebugRenderer().clearPoints();
    }

    if(GameState::ks.get_mouse_right_button())
    {
        GameState::cam.cursor_hidden = true;
        glfwSetCursorPos(window -> getWindow(), GameState::ms.prev_x, GameState::ms.prev_y);
        glfwSetInputMode(window -> getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if(!GameState::ks.get_mouse_right_button())
    {
        //GameState::ms.prev_x = GameState::ms.get_x();
        //GameState::ms.prev_y = GameState::ms.get_y();
        GameState::cam.cursor_hidden = false;
        glfwSetInputMode(window -> getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if(GameState::ks.get_mouse_left_button())
    {
        selector.ProbeSceneObjects(&currScene, static_cast<float>(GameState::ms.click_x), static_cast<float>(GameState::ms.click_y), getWindow(), getRenderer());
        selector.ReadPixel(GameState::ms.click_x, getWindow()->getHeight() - 1 - GameState::ms.click_y);
    }
}

void Editor::updateCamera()
{
    if (!GameState::cam.cursor_hidden)
        return;

    GameState::cam.setCameraLook(GameState::ms.prev_x, GameState::ms.prev_y);
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

Window* Editor::getWindow()
{
    return window;
}

Renderer* Editor::getRenderer()
{
    return &rendol;
}

