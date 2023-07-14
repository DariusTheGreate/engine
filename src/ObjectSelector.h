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

class ObjectSelector
{
public:
    ObjectSelector() = default;

    ObjectSelector(int W, int H);
    
    void ReadPixel(int x, int y);

    glm::vec3 GetRayFromMouse(float mouseX, float mouseY, Window* w);

    bool RaySphere(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 position, float r, Renderer* renderer);

    void ProbeSceneObjects(Scene* scene, float mouseX, float mouseY, Window* w, Renderer* renderer);

private:
    size_t pick_fbo = 0;
    size_t pick_texture = 0;
};

