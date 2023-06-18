#include <Editor.h>

Editor::Editor(Window* wind) : window(wind), ui(wind->getWindow()), rendol(&currScene)
{
    GameState::ms.init(wind->getWidth() / 2, wind->getHeight() / 2);
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
    ui.apply();
   	rendol.updateBuffers(window); 
}

void Editor::updateInput() {
    if (GameState::ks.get_w()) {
        GameState::cam.moveCameraForward();
    }
    if (GameState::ks.get_a()) {
        GameState::cam.moveCameraLeft();
    }
    if (GameState::ks.get_s()) {
        GameState::cam.moveCameraBackward();
    }
    if (GameState::ks.get_d()) {
        GameState::cam.moveCameraRight();
    }

    /*if (GameState::ks.get_2()) {
        GameState::cam.cursor_hidden = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (GameState::ks.get_1()) {
        GameState::cam.cursor_hidden = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window, GameState::cam.getLastX(), GameState::cam.getLastY());
    }
    */
    if (GameState::ks.get_0()) {
        debug_mode = true;

		GameState::debug_msg.append("debug mode toogled\n");
    }
    if (GameState::ks.get_9()) {
        debug_mode = false;

    }

    if(GameState::ks.get_mouse_right_button() != GameState::cam.cursor_hidden){
        glfwSetInputMode(window -> getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window -> getWindow(), GameState::ms.prev_x, GameState::ms.prev_y);
        GameState::cam.cursor_hidden = true;
    }
    if(!GameState::ks.get_mouse_right_button()){
        GameState::ms.prev_x = GameState::ms.get_x();
        GameState::ms.prev_y = GameState::ms.get_y();
        GameState::cam.cursor_hidden = false;
        glfwSetInputMode(window -> getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Editor::updateCamera()
{
    GameState::cam.setCameraLook(GameState::ms.get_x(), GameState::ms.get_y());
    GameState::cam.setScroolState(GameState::ms.get_offset_x(), GameState::ms.get_offset_y());
}

void Editor::setPolygonMode(size_t type) {
    glPolygonMode(GL_FRONT_AND_BACK, type);
}

void Editor::printFPS() {
    double currentTime = glfwGetTime();
    frame_number++;
    if (currentTime - lastTime >= 1.0) {
        std::cout << "framerate: " << 1000.0 / double(frame_number) << "\n";
        frame_number = 0;
        lastTime += 1.0;
    }
}
