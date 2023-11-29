#pragma once
#include <Scene.h>
#include <GameState.h>
#include <Renderer.h>
#include <Mesh.h>
#include <Animation.h>
#include <SystemInfo.h>
#include <Profiler.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include <iostream>
#include <functional>
#include <memory>
#include <cmath>
#include <deque>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class GuizmoTool
{
public:
    void tool(Object* item)
    {
        ImGuizmo::SetDrawlist();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::BeginFrame();

        if(!(!GameState::instance->cam.cursor_hidden && !GameState::instance->ks.get_mouse_right_button())){
            cameraProjection = GameState::instance->cam.getPerspective((int)io.DisplaySize.x, (int)io.DisplaySize.y);//glm::perspective(45.0f, (GLfloat)1600/ (GLfloat)900, 1.0f, 150.0f);
            cameraView = (GameState::instance->cam.getBasicLook());
        }
        
        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), curr_operation, guizmoMode, glm::value_ptr(item->getTransform().matrix));

        if(item->getPointLight()){
            item->getPointLight()->position = item->getTransform().getPosition();
            //item_clicked->getTransform().set_scale(glm::vec3{objScl[0][0], objScl[1][1], objScl[2][2]});
        }
        
        if(item->getParticleSystem()){
            item->getParticleSystem()->emitter = item->getTransform().getPosition();
            //item_clicked->getTransform().set_scale(glm::vec3{objScl[0][0], objScl[1][1], objScl[2][2]});
        }

        auto& objtrref = item->getTransform().matrix;
        item->traverseObjects([&objtrref](Object* obj){
            obj->getTransform().matrix = objtrref;
        });
    } 

    void setOperation(ImGuizmo::OPERATION op)
    {
        curr_operation = op;
    }

    void setMode(ImGuizmo::MODE mode)
    {
        guizmoMode = mode;
    }

    ImGuizmo::MODE getMode()
    {
        return guizmoMode; 
    }

    ImGuizmo::OPERATION getOperation()
    {
        return curr_operation;
    }

private:
    ImGuizmo::OPERATION curr_operation = ImGuizmo::OPERATION::TRANSLATE;
    ImGuizmo::MODE guizmoMode = ImGuizmo::WORLD;

    glm::mat4 cameraProjection = glm::mat4(1.0f);
    glm::mat4 cameraView = glm::mat4(1.0f);
};

class UI {

public:
	UI(GLFWwindow* window, GameState* st);

    void renderUI(Scene& scn, Renderer& r);

    void apply();

    void AssetBrowserWindow();

    //TODO(darius) check this out https://github.com/tksuoran/erhe/blob/7617e6eda85219346aa92c2c980c699e659c359d/src/editor/windows/layers_window.cpp#LL56C4-L56C4
    void sceneWindow(Scene& scene, Renderer& r);

    void showObjectWindow(Object* obj, Renderer& r, Scene& scene);

    void objectManipulationWindow(Scene& scene);

    void componentAdderWindow(Renderer& hui);

    void guizmoWindow();

    void showConsoleWindow();
    
    void showEditorSettingsWindow(Renderer& hui);

    void profilesWindow(std::vector<Profiler<float>>& profilers);

    void sceneCamerasWindow(Scene& scene);

    Object* getItemClicked(); 

    void setItemClicked(Object* obj); 

    void setItemClickedForced(Object* obj) ;

    void setTime(double time);

private:
    bool show_scene_window = false;
    bool show_object_window = false;
    bool show_component_adder = false;

    std::string path{""};
    std::string name{""};

    int emptyCreated = 0;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO* io = nullptr;

	Object* item_clicked = nullptr;
    Object* item_copy = nullptr;


    GameState* state= nullptr;

    int max_name_length = 21;

    double timeVal = 0;

    GuizmoTool guizmoTool;

    //std::deque<float> framerateHistory;
    //size_t historySize = 1000;

    Profiler<float> profile;
};
