#pragma once

#include <iostream>

#include <OpenglWrapper.h>
#include <GameState.h>
#include <Window.h>
#include <Scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ObjectSelector
{
public:
    ObjectSelector() = default;

    ObjectSelector(int W, int H)
    {
        OpenglWrapper::GenerateFrameBuffers(&pick_fbo);
        OpenglWrapper::BindFrameBuffer(pick_fbo);
        OpenglWrapper::GenerateTextures(&pick_texture);
        OpenglWrapper::BindTexture(pick_texture);
        OpenglWrapper::ImageTexture(GL_DEPTH_COMPONENT, W, H, nullptr, GL_TEXTURE_2D, GL_FLOAT);
        OpenglWrapper::ImageFrameBuffer(pick_texture);
        OpenglWrapper::UnbindTexture();
        OpenglWrapper::UnbindFrameBuffer();
    } 
    
    void ReadPixel(int x, int y)
    {
        //std::cout << x << "|" << y << "\n";
        OpenglWrapper::BindFrameBuffer(pick_fbo);
        OpenglWrapper::ReadBuffer();
        int pixel[4] = {0, 0, 0, 0};
        OpenglWrapper::ReadPixels(x, y, &pixel);
        OpenglWrapper::ReadZeroBuffer();
        //std::cout << pixel[0] << " " << pixel[1] << "\n";
        OpenglWrapper::UnbindFrameBuffer();
    }

    glm::vec3 GetRayFromMouse(int mouseX, int mouseY, Window* w)
    {
        glm::vec2 ray_nds = glm::vec2(mouseX, mouseY);
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        glm::mat4 invProjMat = glm::inverse(GameState::cam.getPerspective(w->getWidth(), w->getHeight()));
        glm::vec4 eyeCoords = invProjMat * ray_clip;
        eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
        glm::mat4 invViewMat = glm::inverse(GameState::cam.getBasicLook());
        glm::vec4 rayWorld = invViewMat * eyeCoords;
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

        //std::cout << "Ray: " << rayDirection.x << "|" << rayDirection.y << "|" << rayDirection.z << "\n";

        return rayDirection;
    }

    bool RaySphere(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 position, float r) 
    {
        glm::vec3 v = glm::vec3(position.x, position.y, 0.5f) - GameState::cam.getCameraPos();
        float a = glm::dot(ray_direction, ray_direction); 
        float b = 2 * glm::dot(v, ray_direction);
        float c = glm::dot(v, v) - r*r;

        float b_squared_minus_4ac = b * b - 4 * a * c;

        if (b_squared_minus_4ac > 0)
        {
            float x1 = (-b - sqrt(b_squared_minus_4ac)) / 2.0f;
            float x2 = (-b + sqrt(b_squared_minus_4ac)) / 2.0f;
            if (x1 >= 0.0f && x2 >= 0.0f)
                return true;
            if (x1 < 0.0f && x2 >= 0.0f)
                return true;
        }
        
        return false;
    }

    void ProbeSceneObjects(Scene* scene, int mouseX, int mouseY, Window* w)
    {
        std::cout << "Coords: " << mouseX << "|" << mouseY << "\n";
        GameState::msg("Coords: " + std::to_string(mouseX) + "|" + std::to_string(mouseY) + "\n");
        auto& objects = scene->get_objects();
        for(size_t i = 0; i < objects.size(); ++i)
        {
            if(RaySphere(GameState::cam.getCameraPos(), GetRayFromMouse(mouseX, mouseY, w), objects[i]->getTransform().position, 5)){
                //std::cout << "clicked " << objects[i]->get_name() << "\n"; 
            }
        }
    }

private:
    size_t pick_fbo = 0;
    size_t pick_texture = 0;
};

