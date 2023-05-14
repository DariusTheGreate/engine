#pragma once
#include <Scene.h>
#include <GameState.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <functional>

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

        //testWindow();
        sceneWindow(scn);

        ImGui::Render();
    }

    void apply() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void testWindow()
    {
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_object_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_object_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
            ImGui::End();
        }

        if (show_object_window)
        {
            ImGui::Begin("Another Window", &show_object_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_object_window = false;
            ImGui::End();
        }
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
						bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
                        if (ImGui::IsItemClicked()) {
                            std::cout << "clicked\n";
                            show_object_window = true;
                            item_cicked = objects[i];
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
        }

		ImGui::Begin("Object window", &show_object_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text(obj -> get_name().c_str());
		//if (ImGui::Button("Close Me"))
        //    show_object_window = false;

		if (ImGui::Button("Show Object") && obj) {
			std::cout << obj->get_pos().x;
			auto v = obj->get_pos();
			v += glm::vec3{0, 0, 10};
			GameState::cam.setCameraPos(v);
			GameState::cam.setCameraLook(v.x,v.y);
			GameState::cam.setUnexpectedUpdate(true);
		}
        
		ImGui::Checkbox("Hide Object", &obj -> object_hidden_state());

		ImGui::Checkbox("Static Object", &obj -> getRigidBody().get_is_static_ref()); 

        glm::vec3& objposref = obj->get_pos_ref();

		//ImGui::SliderFloat("float", &objposref.x, 0.0f, 1.0f); 
		//ImGui::SliderFloat("float", &objposref.y, 0.0f, 1.0f); 
		//ImGui::SliderFloat("float", &objposref.z, 0.0f, 1.0f); 
        //ImGui::DragFloat("DragFloat (0 -> 1)", &objposref.x, 0.005f, 0.0f, 1.0f, "%.3f", ImGuiColorEditFlags_NoAlpha);
        ImGui::Text("position vector %f, %f, %f", objposref.x, objposref.y, objposref.z);
        ImGui::DragFloat("position x", &objposref.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position y", &objposref.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position z", &objposref.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        glm::quat& objposquat = obj->getRigidBody().get_orientation_quaternion_ref();

        ImGui::Text("orientation quaternion %f, %f, %f, %f", objposquat.x, objposquat.y, objposquat.z, objposquat.w);
        ImGui::DragFloat("orientation x", &objposquat.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation y", &objposquat.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation z", &objposquat.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation w", &objposquat.w, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        
		ImGui::Text("orientation angles %f, %f, %f", &obj->getRigidBody().get_ex(), &obj->getRigidBody().get_ey(), &obj->getRigidBody().get_ez());
        ImGui::DragFloat("orientation x", &obj->getRigidBody().get_ex(), 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation y", &obj->getRigidBody().get_ey(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation z", &obj->getRigidBody().get_ez(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        obj->getRigidBody().set_quat_from_angles();

		ImGui::End();
    }

private:
    bool show_demo_window = false;
    bool show_scene_window = false;
    bool show_object_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO* io;

	Object* item_cicked = nullptr;
};

