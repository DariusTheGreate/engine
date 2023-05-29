#pragma once
#include <Scene.h>
#include <GameState.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include <iostream>
#include <functional>
#include <memory>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class UI {

public:
	UI(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO(); (void)io;
        io -> ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io -> ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
       // ImGui::StyleColorsLight();

        const char* glsl_version = "#version 130";
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
	}

    void renderUI(Scene& scn) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        sceneWindow(scn);
        showConsoleWindow();

        ImGui::Render();
    }

    void apply() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    //TODO(darius) MKAE IT ITERATIVE BFS(use Object -> traverseObjects()) 
    //TODO(darius) check this out https://github.com/tksuoran/erhe/blob/7617e6eda85219346aa92c2c980c699e659c359d/src/editor/windows/layers_window.cpp#LL56C4-L56C4
    void sceneWindow(Scene& scene)
    {
        const ImGuiTreeNodeFlags parent_flags{
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanFullWidth
        };
        const ImGuiTreeNodeFlags leaf_flags{
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_NoTreePushOnOpen |
            ImGuiTreeNodeFlags_Leaf
        };
	    
        if (ImGui::Begin("Game Ojects", &show_scene_window))//, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::LabelText("", "Game Objects in Scene.");

            if (ImGui::TreeNodeEx("Scene objects", parent_flags))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);
                auto objects = scene.get_objects();
                for (int i = 0; i < objects.size(); i++)
                {
                    if (!objects[i])
                        continue;

                    ImGuiTreeNodeFlags node_flags = objects[i]->get_child_objects().empty() ? leaf_flags : parent_flags;

                    if (ImGui::TreeNodeEx(objects[i]->get_name().c_str(), parent_flags)) {
						objects[i]->traverseObjects([&](Object* op) {
							ImGuiTreeNodeFlags node_flags = op->get_child_objects().empty() ? leaf_flags : parent_flags;
							if (ImGui::TreeNodeEx(op->get_name().c_str(), parent_flags)) {
								bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
								if (ImGui::IsItemClicked()) {
									std::cout << "clicked " << (size_t)op << "\n";
									show_object_window = true;
                                    item_cicked = op;
                                    objTr = item_cicked->getRigidBody().tr.get_quatmat();
                                    auto pos = item_cicked->getRigidBody().tr.position; 
                                    objTr[3][0] = pos.x;
                                    objTr[3][1] = pos.y;
                                    objTr[3][2] = pos.z;
								}
								ImGui::TreePop();
							}
						});

                        //TODO(darius) add another traversal to scene
						bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
                        if (ImGui::IsItemClicked()) {
                            show_object_window = true;
                            item_cicked = objects[i];
                            objTr = item_cicked->getRigidBody().tr.get_quatmat();
                            auto pos = item_cicked->getRigidBody().tr.position; 
                            objTr[3][0] = pos.x;
                            objTr[3][1] = pos.y;
                            objTr[3][2] = pos.z;
                        }

						ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
                ImGui::PopStyleVar();
            }
        }
        
        ImGui::End();

		if (show_object_window)
        {
            showObjectWindow(item_cicked);
	    }

    }

    void showObjectWindow(Object* obj)
    {
        if (!obj) {
            std::cout << "no obj provided. exsiting showObjectWindow(obj)\n";
            return;
        }

		ImGui::Begin("Object window", &show_object_window);
		ImGui::Text(obj -> get_name().c_str());

		if (ImGui::Button("Show Object") && obj) {
			std::cout << obj->get_pos().x;
			auto v = obj->get_pos();
			v += glm::vec3{0, 0, 10};
			GameState::cam.setCameraPos(v);
			GameState::cam.setUnexpectedUpdate(true);
		}
        
		ImGui::Checkbox("Hide Object", &obj -> object_hidden_state());
		ImGui::Checkbox("Static Object", &obj -> getRigidBody().get_is_static_ref()); 

        //TODO(darius) so objTr updates quaternion at the end of UI work, cause of Guizmos. This is ugly

        ImGui::Text("position vector %f, %f, %f", objTr[3][0], objTr[3][1], objTr[3][2]);
        ImGui::DragFloat("position x", &objTr[3][0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position y", &objTr[3][1], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position z", &objTr[3][2], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        auto q = glm::quat(objTr);
        ImGui::Text("orientation quaternion %f, %f, %f, %f", q.x, q.y, q.z, q.w);

		ImGui::Text("orientation angles %f, %f, %f", &obj->getRigidBody().get_ex(), &obj->getRigidBody().get_ey(), &obj->getRigidBody().get_ez());
        ImGui::DragFloat("orientation x", &obj->getRigidBody().get_ex(), 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation y", &obj->getRigidBody().get_ey(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation z", &obj->getRigidBody().get_ez(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        obj->getRigidBody().set_quat_from_angles();

        glm::vec3& coliderSizeRef = obj->getColider().get_size_ref();
        ImGui::Text("colider size %f, %f, %f", coliderSizeRef.x, coliderSizeRef.y, coliderSizeRef.z);
        ImGui::DragFloat("size x", &coliderSizeRef.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("size y", &coliderSizeRef.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("size z", &coliderSizeRef.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        std::vector<ScriptProperty<glm::vec3>>& vecProperties = obj->getScript().getVectorProperties();

        for(int i = 0; i < vecProperties.size(); ++i){
            ImGui::Text((vecProperties[i].name).c_str());
            ImGui::DragFloat((vecProperties[i].name + " x").c_str(), &vecProperties[i]->x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat((vecProperties[i].name + " y").c_str(), &vecProperties[i]->y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat((vecProperties[i].name + " z").c_str(), &vecProperties[i]->z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        }

        guizmoWindow();

		ImGui::End();
    }

    void guizmoWindow()
    {
        //ImGui::Begin("Editor");
        //IMPORTANT!

        if (ImGui::Button("Translation tool"))
        {
            curr_operation = ImGuizmo::OPERATION::TRANSLATE;
        }

        if (ImGui::Button("Rotation tool"))
        {
            curr_operation = ImGuizmo::OPERATION::ROTATE;
        }
        
        ImGuizmo::SetDrawlist();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::BeginFrame();

        if(!(!GameState::cam.cursor_hidden && !GameState::ks.get_mouse_right_button())){
            cameraProjection = GameState::cam.getPerspective(1600, 900);//glm::perspective(45.0f, (GLfloat)1600/ (GLfloat)900, 1.0f, 150.0f);
    		cameraView = (GameState::cam.getBasicLook());
        }

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), curr_operation, ImGuizmo::LOCAL, glm::value_ptr(objTr));

        //TODO(darius) make it work for entities. Just do traverse, but it look garbage, consider making wome wrapper
        item_cicked->getRigidBody().tr.set_from_quatmat(objTr);
        auto& objtrref = objTr;
        item_cicked->traverseObjects([&objtrref](Object* obj){
            obj->getRigidBody().tr.set_from_quatmat(objtrref);
        });


		int viewManipulateRight = ImGui::GetWindowPos().x + 900;
		int viewManipulateTop = ImGui::GetWindowPos().y;
        //ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(glm::mat4(1)), 100.f);
		ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), 8, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

        //ImGui::End();
    }

    void showConsoleWindow()
    {
        ImGui::Begin("Console");

        if (ImGui::Button("Clear console")){
            GameState::clear_msg();
        }

        ImGui::TextWrappedV(GameState::debug_msg.c_str(), 0);

        //ImGui::SetScrollY(GameState::debug_len*10);

        ImGui::End();
    }
private:
    bool show_demo_window = false;
    bool show_scene_window = false;
    bool show_object_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO* io;

	Object* item_cicked = nullptr;
    glm::mat4 objTr = { 1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f };

    ImGuizmo::OPERATION curr_operation = ImGuizmo::OPERATION::TRANSLATE;
    glm::mat4 cameraProjection;
    glm::mat4 cameraView;
};
