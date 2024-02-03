#include "InputManager.h"
#include <Engine/Editor.h>

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        GameState::instance->ks.set_w(true);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        GameState::instance->ks.set_s(true);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        GameState::instance->ks.set_a(true);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        GameState::instance->ks.set_d(true);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        GameState::instance->ks.set_q(true);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        GameState::instance->ks.set_e(true);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        GameState::instance->ks.set_c(true);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        GameState::instance->ks.set_l(true);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        GameState::instance->ks.set_lshift(true);
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        GameState::instance->ks.set_0(true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        GameState::instance->ks.set_1(true);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        GameState::instance->ks.set_2(true);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        GameState::instance->ks.set_3(true);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        GameState::instance->ks.set_4(true);
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        GameState::instance->ks.set_9(true);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        GameState::instance->ks.set_cntrl(true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        GameState::instance->ks.set_w(false);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        GameState::instance->ks.set_s(false);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        GameState::instance->ks.set_a(false);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        GameState::instance->ks.set_d(false);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
        GameState::instance->ks.set_q(false);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        GameState::instance->ks.set_e(false);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        GameState::instance->ks.set_c(false);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        GameState::instance->ks.set_l(false);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
        GameState::instance->ks.set_lshift(false);
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
        GameState::instance->ks.set_0(false);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        GameState::instance->ks.set_1(false);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
        GameState::instance->ks.set_2(false);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        GameState::instance->ks.set_3(false);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
        GameState::instance->ks.set_4(false);
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
        GameState::instance->ks.set_9(false);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
        GameState::instance->ks.set_cntrl(false);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        GameState::instance->ks.set_mouse_right_button(true);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glfwSetCursorPos(window, GameState::ms.prev_x, GameState::ms.prev_y);

        //std::cout << "press right\n";
        //save click position

        //GameState::ms.prev_x = GameState::ms.get_x();
        //GameState::ms.prev_y = GameState::ms.get_y();
        //GameState::cam.cursor_hidden = true;
    }
    //this event HAPPENS every frame
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        GameState::instance->ks.set_mouse_right_button(false);
        //GameState::ms.prev_x = GameState::ms.get_x();
        //GameState::ms.prev_y = GameState::ms.get_y();
        //GameState::cam.cursor_hidden = false;
        //
        //glfwSetCursorPos(window, GameState::ms.prev_x, GameState::ms.prev_y);
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        //std::cout << "unpress right\n";
        //place into prev saved click position
        //GameState::ms.set_x(GameState::ms.prev_x);
        //GameState::ms.set_y(GameState::ms.prev_y);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        GameState::instance->ks.set_mouse_left_button(true);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        GameState::instance->ks.set_mouse_left_button(false);
    }
}

void InputManager::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!GameState::instance->cam.cursor_hidden) {
        //    return;
            //std::cout << "here " << xposIn << "|" << yposIn << "\n";
        GameState::ms.click_x = static_cast<int>(xposIn);
        GameState::ms.click_y = static_cast<int>(yposIn);
    }

    GameState::ms.set_x((int)xposIn);
    //GameState::msg(std::to_string(GameState::ms.get_x()) + "\n");
    GameState::ms.set_y((int)yposIn);
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameState::ms.set_offset_x((int)xoffset);
    GameState::ms.set_offset_y((int)(yoffset));
}

void InputManager::drop_callback(GLFWwindow* window, int count, const char** paths)
{
    for (size_t i = 0; i < count; i++) {
        Editor::fileDropCallbackDispatch(paths[i]);
    }
}