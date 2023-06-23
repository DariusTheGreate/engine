#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Camera {
public:

    void setCameraLook(double xposIn, double yposIn);

    void setCameraPos(glm::vec3 pos_in);

    void setScroolState(double xoffset, double yoffset);

    void moveCameraLeft();

    void moveCameraRight();

    void moveCameraForward();

    void moveCameraBackward();

    //NOTE(darius) its actually projection matrix.
    glm::mat4 getPerspective(int w, int h) const;

    //NOTe(darius) its actually view matrix
    glm::mat4 getBasicLook() const;
    
    glm::vec3 getCameraPos() const;

    float getLastX();

    float getLastY();

    void setUnexpectedUpdate(bool in);

    bool getUnexpectedUpdate();

    std::vector<float> getRawView() const;

    glm::mat4 getView() const;

    glm::vec4 getRawViewPort(int w, int h);

public:
    bool cursor_hidden = true;

private:
    //TODO(darius) make it transform
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;

	float cameraSpeed = 0.5f;
	float lastFrame = 0.0f;

    bool unexpectedUpdate = false;
};
