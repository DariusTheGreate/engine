#pragma once
#include <Scene.h>
#include <GameState.h>
#include <Renderer.h>
#include <Mesh.h>
#include <LightingShaderRoutine.h>
#include <SkeletalAnimationShaderRoutine.h>
#include <Animator.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ImGuizmo.h>

#include <iostream>
#include <functional>
#include <memory>

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class UI {

public:
	UI(GLFWwindow* window, GameState* st) {
        state = st;
		IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = &ImGui::GetIO(); (void)io;
        io -> ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io -> ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io -> ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io -> ConfigDragClickToInputText = true;

        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        const char* glsl_version = "#version 130";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
	}

    void renderUI(Scene& scn, Renderer& r) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //TODO(darius) uncomment this in order to enable docking windows. But it brokes color of application idk (mb cause of srgb enbled but i tryed to disable it, and that didnt worked)
        //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        
        sceneWindow(scn, r);
        showConsoleWindow();

        //ImGui::EndFrame();
        ImGui::Render();
        //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void apply() {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    //TODO(darius) check this out https://github.com/tksuoran/erhe/blob/7617e6eda85219346aa92c2c980c699e659c359d/src/editor/windows/layers_window.cpp#LL56C4-L56C4
    void sceneWindow(Scene& scene, Renderer& r)
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
	    
        if (ImGui::Begin("Game Objects", &show_scene_window))//, ImGuiWindowFlags_AlwaysAutoResize))
        {

            if (ImGui::Button("Add Object")) {
                scene.AddEmpty(emptyCreated++);        
            }

            ImGuiDragDropFlags src_flags = 0;
            src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
            src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
            //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
                    

            ImGui::LabelText("", "Game Objects in Scene.");

            if (ImGui::TreeNodeEx("Scene objects", parent_flags))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);
                auto objects = scene.get_objects();
                for (int i = 0; i < objects.size(); i++)
                {
                    if (!objects[i])
                        continue;

                    //TODO(darius) selectalbes here? This chunk can be used to create selectable think
                    /*
                    ImGui::Selectable(objects[i]->get_name().c_str());
                    if (ImGui::BeginDragDropSource(src_flags))
                    {
                        if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip)){

                        }

                        ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
                        ImGui::EndDragDropSource();
                    }
                    */

                    ImGuiTreeNodeFlags node_flags = objects[i]->get_child_objects().empty() ? leaf_flags : parent_flags;

                    if (ImGui::TreeNodeEx(objects[i]->get_name().c_str(), parent_flags)) {
						objects[i]->traverseObjects([&](Object* op) {
							ImGuiTreeNodeFlags node_flags = op->get_child_objects().empty() ? leaf_flags : parent_flags;
							if (ImGui::TreeNodeEx(op->get_name().c_str(), parent_flags)) {
								bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
								if (ImGui::IsItemClicked()) {
									show_object_window = true;
                                    item_cicked = op;
                                    if(item_cicked->getRigidBody())
                                        objTr = item_cicked->getRigidBody()->tr.get_quatmat();
                                    auto pos = item_cicked->getTransform().position; 
                                    objTr[3][0] = pos.x;
                                    objTr[3][1] = pos.y;
                                    objTr[3][2] = pos.z;
								}
								ImGui::TreePop();
							}
						});

						bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
                        if (ImGui::IsItemClicked()) {
                            show_object_window = true;
                            item_cicked = objects[i];
                            if(item_cicked->getRigidBody())
                                objTr = item_cicked->getRigidBody()->tr.get_quatmat();
                            auto pos = item_cicked->getTransform().position; 
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
            showObjectWindow(item_cicked, r, scene);
	    }

    }

    void showObjectWindow(Object* obj, Renderer& r, Scene& scene)
    {
        if (!obj) {
            std::cout << "no obj provided. exsiting showObjectWindow(obj)\n";
            return;
        }

		ImGui::Begin("Object window", &show_object_window);
		ImGui::Text(obj -> get_name().c_str());

		if (ImGui::Button("Show Object") && obj) {
			auto v = obj->get_pos();
			v += glm::vec3{0, 0, 10};
			GameState::cam.setCameraPos(v);
			GameState::cam.setUnexpectedUpdate(true);
		}
        
		ImGui::Checkbox("Hide Object", &obj -> object_hidden_state());
        if(obj->getRigidBody())
    		ImGui::Checkbox("Static Object", &obj -> getRigidBody()->get_is_static_ref()); 

        //TODO(darius) so objTr updates quaternion at the end of UI work, cause of Guizmos. This is ugly

        auto tr = obj->getTransform();
        if (ImGui::CollapsingHeader("Transform component")){
            ImGui::Text("position %f, %f, %f", objTr[3][0], objTr[3][1], objTr[3][2]);
            ImGui::DragFloat("position x", &objTr[3][0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position y", &objTr[3][1], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position z", &objTr[3][2], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

            ImGui::Text("scale %f, %f, %f", tr.scale.x, tr.scale.y, tr.scale.z);
            ImGui::DragFloat("scale x", &tr.scale.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("scale y", &tr.scale.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("scale z", &tr.scale.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        }

        //auto q = glm::quat(objTr);
        //ImGui::Text("orientation quaternion %f, %f, %f, %f", q.x, q.y, q.z, q.w);

		/*ImGui::Text("orientation angles %f, %f, %f", &obj->getRigidBody().get_ex(), &obj->getRigidBody().get_ey(), &obj->getRigidBody().get_ez());
        ImGui::DragFloat("orientation x", &obj->getRigidBody().get_ex(), 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation y", &obj->getRigidBody().get_ey(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orientation z", &obj->getRigidBody().get_ez(), 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        obj->getRigidBody().set_quat_from_angles();
        */

/*
    std::optional<RigidBody> rbody;
*/

        //TODO(darius) custumize bar - https://stackoverflow.com/questions/73626738/in-imgui-is-it-possible-to-change-the-icon-at-the-left-of-a-collapsing-header

        auto& body = obj->getRigidBody();
        if (body && ImGui::CollapsingHeader("RigidBody component")){
            ImGui::DragFloat("mass", &body->mass, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        } 

        auto& particles = obj->getParticleSystem();
        if(particles && ImGui::CollapsingHeader("ParticleSystem component")){
            ImGui::DragFloat("emitter x", &particles->emitter.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("emitter y", &particles->emitter.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("emitter z", &particles->emitter.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("size x", &particles->particle_size.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("size y", &particles->particle_size.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("size z", &particles->particle_size.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("max boundary", &particles->maxBound, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("min boundary", &particles->minBound, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::Checkbox("pause", &particles->pause);

            ImGui::Text("particles count %i", particles->positions.size());

            if(ImGui::CollapsingHeader("Particles Material")){
                ImGui::DragFloat("ambient x", &particles->particleMaterial->ambient.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("ambient y", &particles->particleMaterial->ambient.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("ambient z", &particles->particleMaterial->ambient.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

                ImGui::DragFloat("diffuse x", &particles->particleMaterial->diffuse.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("diffuse y", &particles->particleMaterial->diffuse.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("diffuse z", &particles->particleMaterial->diffuse.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            }

            if(ImGui::Button("Add Particle")){
                particles->addPositions(100);             
            }
        }

        auto& material = obj->getMaterial();
        if (material && ImGui::CollapsingHeader("Material component")){
            ImGui::DragFloat("ambient x", &material->ambient.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("ambient y", &material->ambient.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("ambient z", &material->ambient.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);


            ImGui::DragFloat("diffuse x", &material->diffuse.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("diffuse y", &material->diffuse.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("diffuse z", &material->diffuse.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("specular x", &material->specular.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("specular y", &material->specular.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("specular z", &material->specular.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("shininess", &material->shininess, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        } 

        auto& animator = obj->getAnimator(); 

        if (animator && ImGui::CollapsingHeader("Animator component")){
            ImGui::Text("timestamps %f, %f", animator->getCurrTime(), animator->getDeltaTime());
        }

        auto& collider = obj->getColider();
        if (collider && ImGui::CollapsingHeader("Colider component")){
            glm::vec3& coliderSizeRef = collider->get_size_ref();
            ImGui::Text("colider size %f, %f, %f", coliderSizeRef.x, coliderSizeRef.y, coliderSizeRef.z);
            ImGui::DragFloat("size x", &coliderSizeRef.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("size y", &coliderSizeRef.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("size z", &coliderSizeRef.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

            glm::vec3& coliderPoint = collider->get_render_shift();
            ImGui::Text("colider point%f, %f, %f", coliderPoint.x, coliderPoint.y, coliderPoint.z);
            ImGui::DragFloat("point x", &coliderPoint.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("point y", &coliderPoint.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("point z", &coliderPoint.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        }

        auto& script = obj->getScript();
        if (script && ImGui::CollapsingHeader("Script component")){

            std::vector<ScriptProperty<glm::vec3>>& vecProperties = script->getVectorProperties();
            for(int i = 0; i < vecProperties.size(); ++i){
                if (ImGui::CollapsingHeader((vecProperties[i].name).c_str())){
                    ImGui::DragFloat((vecProperties[i].name + " x").c_str(), &vecProperties[i]->x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                    ImGui::DragFloat((vecProperties[i].name + " y").c_str(), &vecProperties[i]->y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                    ImGui::DragFloat((vecProperties[i].name + " z").c_str(), &vecProperties[i]->z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                }
            }

            std::vector<ScriptProperty<float>>& floatProperties = script->getFloatProperties();
            for(int i = 0; i < floatProperties.size(); ++i){
                if (ImGui::CollapsingHeader((floatProperties[i].name).c_str())){
                    ImGui::DragFloat((floatProperties[i].name).c_str(), floatProperties[i].val, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
                }
            }
        }

        auto& light = obj->getPointLight();
        if(light && ImGui::CollapsingHeader("PointLight"))
        {
            ImGui::DragFloat("position R", &light->position.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position G", &light->position.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position B", &light->position.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::ColorEdit3("color", (float*)&light->color);
            /*light->color.x = clear_color.x;
            light->color.y = clear_color.y;
            light->color.z = clear_color.z;
            */ 
            //ImGui::DragFloat("color R", &light->color.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            //ImGui::DragFloat("color G", &light->color.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            //ImGui::DragFloat("color B", &light->color.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("color factor", &light->colorFactor, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("diffuse factor", &light->diffuseFactor, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            ImGui::DragFloat("specular x", &light->specular.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("specular y", &light->specular.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("specular z", &light->specular.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        }

        if(ImGui::CollapsingHeader("Add Component")){
            show_component_adder = true;
            ImGui::Indent();
            componentAdderWindow(r);
            ImGui::Unindent();
        }

        if(ImGui::Button("Add Child Object")){
            Object* child = scene.createSubobject(obj, emptyCreated++);
        }

        guizmoWindow();

		ImGui::End();
    }

    void componentAdderWindow(Renderer& hui)
    {
        if(!show_component_adder || !item_cicked)
            return;

        //ImGui::Begin("Add Component");

        if(ImGui::Button("Collider"))
            item_cicked->addCollider();

        if(item_cicked->getModel() && ImGui::Button("PointLight")){
            item_cicked->addPointLight();    
        }

        if(item_cicked->getModel() && ImGui::Button("ParticleSystem")){
            FlatMesh flat;
            flat.setTexture("E:own/programming/engine/textures", "dean.png");

            Shader particleVertex = Shader("E:/own/programming/engine/shaders/particleVertexShader.glsl", GL_VERTEX_SHADER);
            Shader particleFragment = Shader("E:/own/programming/engine/shaders/particleFragmentShader.glsl", GL_FRAGMENT_SHADER);

            particleVertex.compile();
            particleFragment.compile();
            particleVertex.link(particleFragment);

            LightingShaderRoutine shaderRroutine = LightingShaderRoutine(Shader(particleVertex)); //hui.getCurrShaderRoutine();

            Material m;
            m.ambient = {1,1,1};
            m.diffuse = {1,1,1};
            m.specular = {1,1,1};
            m.shininess = 32;

            ParticleSystem ps = ParticleSystem();
            ps.addParticle(FlatMesh(flat), Shader(particleVertex), LightingShaderRoutine(shaderRroutine), Material(m));

            item_cicked->addParticleSystem(std::move(ps));    
        }

        if(item_cicked->getModel() && ImGui::Button("Material"))
        {
            Material m;

            m.ambient = {1,0,1};
            m.diffuse = {1,0,1};
            m.specular = {1,0,1};
            m.shininess = 32;

            item_cicked->setMaterial(m);
        }

        if(ImGui::CollapsingHeader("Model")){
            ImGui::Indent();

            size_t routine = hui.getShaderRoutine();
            Shader shader = hui.getShader();

            LightingShaderRoutine shadeRroutine = {Shader(shader)};

            if(ImGui::CollapsingHeader("ModelFile")){
                ImGui::Indent();

                path.resize(100);
                ImGui::InputText("path", (char*)path.c_str(), 100);
                if(ImGui::Button("Load"))
                    item_cicked->addModel(path, shader, shadeRroutine);

                ImGui::Unindent();
            }

            if(ImGui::CollapsingHeader("Animated ModelFile")){
                ImGui::Indent();

                path.resize(100);
                ImGui::InputText("path", (char*)path.c_str(), 100);
                if(ImGui::Button("Load")){
                    Shader animVertex = Shader("E:own/programming/engine/shaders/skeletalAnimationVertexShader.glsl", GL_VERTEX_SHADER);
                    Shader animFragment = Shader("E:own/programming/engine/shaders/skeletalAnimationFragmentShader.glsl", GL_FRAGMENT_SHADER);
                    animVertex.compile();
                    animFragment.compile();
                    animVertex.link(animFragment);

                    item_cicked->addModel(path);
                    Animation* danceAnimation = new Animation(path, &item_cicked->getModel().value());
                    item_cicked->setAnimator(danceAnimation);
                    SkeletalAnimationShaderRoutine animationRroutine = SkeletalAnimationShaderRoutine(Shader(animVertex), &item_cicked->getAnimator().value());
                    item_cicked->getModel()->setShader(Shader(animVertex));
                    item_cicked->getModel()->setAnimationShaderRoutine(animationRroutine);
                }

                ImGui::Unindent();
            }
                
            if(ImGui::Button("Cube")){
                CubeMesh cube;
                cube.setDrawMode(DrawMode::DRAW_AS_ARRAYS);
                item_cicked->addModel(cube, shader, shadeRroutine);
            }

            if(ImGui::Button("Empty")){
                item_cicked->addModel(shader, shadeRroutine);
            }

            ImGui::Unindent();
        }

        /*if(item_cicked->getModel() && ImGui::Button("Animation")){
            Animation* danceAnimation = new Animation("../../../meshes/animations/bot/bot.dae", &item_cicked->getModel().value());
            item_cicked->setAnimator(danceAnimation);
        }
        */

        //ImGui::End();
    } 

    void guizmoWindow()
    {
        if (ImGui::Button("Translation tool"))
        {
            curr_operation = ImGuizmo::OPERATION::TRANSLATE;
        }

        if (ImGui::Button("Rotation tool"))
        {
            curr_operation = ImGuizmo::OPERATION::ROTATE;
        }

        if (ImGui::Button("Scale tool"))
        {
            curr_operation = ImGuizmo::OPERATION::SCALE;
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

        //TODO(darius) make it separate function to change all stuff of object
        item_cicked->getTransform().set_from_quatmat(objTr);
        if(item_cicked->getPointLight()){
            item_cicked->getPointLight()->position = item_cicked->getTransform().position;
        }
        auto& objtrref = objTr;
        item_cicked->traverseObjects([&objtrref](Object* obj){
            obj->getTransform().set_from_quatmat(objtrref);
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

        if (state && ImGui::Button("Clear console")){
            state -> clear_msg();
        }

        if(state)
			ImGui::TextWrappedV(state->debug_msg.c_str(), 0);

        //ImGui::SetScrollY(GameState::debug_len*10);

        ImGui::End();
    }
private:
    bool show_demo_window = false;
    bool show_scene_window = false;
    bool show_object_window = false;
    bool show_component_adder = false;

    std::string path{""};

    int emptyCreated = 0;

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

    GameState* state= nullptr;
};
