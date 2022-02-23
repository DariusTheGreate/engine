#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:

    void setCameraLook(double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        fov -= (float)yoffset;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;
    }

    void moveCameraLeft() {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void moveCameraRight() {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void moveCameraForward() {
        cameraPos += cameraFront * cameraSpeed;
    }

    void moveCameraBackward() {
        cameraPos -= cameraFront * cameraSpeed;
    }

    glm::mat4 getPerspective(int w, int h) {
        return glm::perspective(glm::radians(fov), (float)w / (float)h, 0.1f, 100.0f);
    }

    glm::mat4 getBasicLook() {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

private:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;

	float cameraSpeed = 0.1f;
	float lastFrame = 0.0f;
};