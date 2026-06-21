#pragma once

#include <iostream>

#include <Rendering/OpenglWrapper.h>
#include <Engine/GameState.h>
#include <Engine/Window.h>
#include <Engine/Scene.h>
#include <Rendering/Renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

class Object;

//NOTE(darius) result of a single picking-buffer read: which object and which
//sub-mesh within that object's Model were under the cursor. meshIndex is only
//meaningful when the EDITOR_ID stage encoded it (it always does now).
struct PickResult
{
    int objectID = -1;
    int meshIndex = -1;
};

class ObjectSelector
{
public:
    ObjectSelector() = default;

    ObjectSelector(int W, int H);

    int ReadPixel(int x, int y);

    PickResult ReadPick(int x, int y);

    glm::vec3 GetRayFromMouse(float mouseX, float mouseY, Window* w);

    bool RaySphere(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 position, float r, Renderer* renderer);

    void ProbeSceneObjects(Scene* scene, float mouseX, float mouseY, Window* w, Renderer* renderer);

    static size_t pick_fbo;

    static unsigned int pick_texture;

    //TODO(darius) its not obvious that buffer for picking stored here.
    static FrameBuffer buff;

private:
};

