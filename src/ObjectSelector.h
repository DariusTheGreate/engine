#pragma once

#include <iostream>

#include <OpenglWrapper.h>
#include <GameState.h>
#include <Window.h>
#include <Scene.h>
#include <Renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

class Object;

class ObjectSelector
{
public:
    ObjectSelector() = default;

    ObjectSelector(int W, int H);
    
    int ReadPixel(int x, int y);

    glm::vec3 GetRayFromMouse(float mouseX, float mouseY, Window* w);

    bool RaySphere(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 position, float r, Renderer* renderer);

    void ProbeSceneObjects(Scene* scene, float mouseX, float mouseY, Window* w, Renderer* renderer);

    static size_t pick_fbo;

    static unsigned int pick_texture;

    //TODO(darius) its not obvious that buffer for picking stored here.
    static FrameBuffer buff;

private:
};

