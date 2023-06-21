#include "InputManager.h"

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputManager::standart_input_processor(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void InputManager::framebuffer_size_standart_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void InputManager::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        GameState::ks.set_w(true);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        GameState::ks.set_s(true);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        GameState::ks.set_a(true);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        GameState::ks.set_d(true);
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        GameState::ks.set_0(true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        GameState::ks.set_1(true);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        GameState::ks.set_2(true);
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        GameState::ks.set_9(true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        GameState::ks.set_w(false);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        GameState::ks.set_s(false);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        GameState::ks.set_a(false);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        GameState::ks.set_d(false);
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
        GameState::ks.set_0(false);
    }
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        GameState::ks.set_1(false);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        GameState::ks.set_2(false);
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
        GameState::ks.set_9(false);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        GameState::ks.set_mouse_right_button(true);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glfwSetCursorPos(window, GameState::ms.prev_x, GameState::ms.prev_y);
        //GameState::cam.cursor_hidden = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE){
        GameState::ks.set_mouse_right_button(false);
        //GameState::ms.prev_x = GameState::ms.get_x();
        //GameState::ms.prev_y = GameState::ms.get_y();
        //GameState::cam.cursor_hidden = false;
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        GameState::ks.set_mouse_left_button(true);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        GameState::ks.set_mouse_left_button(false);
    }

}

void InputManager::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!GameState::cam.cursor_hidden)
        return;

    GameState::ms.set_x((int)xposIn);
    GameState::ms.set_y((int)yposIn);
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameState::ms.set_offset_x((int)xoffset);
    GameState::ms.set_offset_y((int)(yoffset));
}
