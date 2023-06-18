#include <Camera.h>

void Camera::setCameraLook(double xposIn, double yposIn)
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

void Camera::setCameraPos(glm::vec3 pos_in)
{
    cameraPos = pos_in;
}

void Camera::setScroolState(double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::moveCameraLeft() {
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveCameraRight() {
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Camera::moveCameraForward() {
    cameraPos += cameraFront * cameraSpeed;
}

void Camera::moveCameraBackward() {
    cameraPos -= cameraFront * cameraSpeed;
}

glm::mat4 Camera::getPerspective(int w, int h) const {
    return glm::perspective(glm::radians(fov), (float)w / (float)h, 0.1f, 100.0f);
}

glm::mat4 Camera::getBasicLook() const {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::vec3 Camera::getCameraPos() const {
    return cameraPos;
}

float Camera::getLastX()
{
    return lastX;
}

float Camera::getLastY()
{
    return lastY;
}

void Camera::setUnexpectedUpdate(bool in) {
    unexpectedUpdate = in;
}

bool Camera::getUnexpectedUpdate() {
    return unexpectedUpdate;
}

std::vector<float> Camera::getRawView() const
{
    std::vector<float> m16 =
    { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };

    return m16;
}

glm::mat4 Camera::getView() const
{
    glm::mat4 viewRotation = { 1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f };

    return viewRotation;
}

glm::vec4 Camera::getRawViewPort(int w, int h)
{
    glm::vec4 viewport(0.0f, 0.0f, w, h);
    return viewport;
}
