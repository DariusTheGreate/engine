#include <UI.h>
#include <Terrain.h>
#include <StackTrace.h>

struct Indenter
{
    Indenter(float dx_in) : dx(dx_in){
        ImGui::Indent(dx);
    }

    ~Indenter(){
        ImGui::Unindent(dx);
    }

    float dx;
};

UI::UI(GLFWwindow* window, GameState* st) {
    state = st;
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    //io -> ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io -> ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io -> ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io -> ConfigDragClickToInputText = true;

    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    profile.setName("FPS");
    
}

void UI::renderUI(Scene& scn, Renderer& r) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //TODO(darius) uncomment this in order to enable docking windows. But it brokes color of application cause you need to switch framebuffers
    //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    
    sceneWindow(scn, r);
    objectManipulationWindow(scn);
    guizmoWindow();
    showConsoleWindow();
    showEditorSettingsWindow(r);
    sceneCamerasWindow(scn);
    AssetBrowserWindow();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //glBindFramebuffer(GL_FRAMEBUFFER, renderer.FBO);
}

void UI::apply() {
    //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::AssetBrowserWindow()
{
    ImGui::Begin("resources");
    ImGui::BeginChild("path");
    ImGui::SameLine();
    ImGui::TextUnformatted(">");
    ImGui::EndChild();
    ImGui::End();
}

//TODO(darius) check this out https://github.com/tksuoran/erhe/blob/7617e6eda85219346aa92c2c980c699e659c359d/src/editor/windows/layers_window.cpp#LL56C4-L56C4
void UI::sceneWindow(Scene& scene, Renderer& r)
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

        if (ImGui::Button("batch objects")) {
            scene.batchProbeSimilarObjects();
        }
        if (ImGui::Button("LOD objects")) {
            scene.updateObjectsLODs();
        }
        if(ImGui::Button("create Terrain")) {
            Terrain t(GameState::engine_path + "textures/height.png");
            scene.addTerrain(std::move(t));
        }

        if (ImGui::Button("recover batched objects")) {
            scene.recoverBatchedObjects();
        }

        ImGui::Text("onjects count %i", scene.getObjects().size());

        if (ImGui::Button("Add Object")) {
            scene.AddEmpty(emptyCreated++);        
        }

        if (ImGui::Button("Add Entity")) {
            auto* enttobj = scene.AddEmpty(emptyCreated++);        
            scene.createEntity(enttobj, path, false);
        }

        if (ImGui::Button("Add Objects")) {
            //E:\own\programming\engine\meshes\supersponza\Sponza\sponza.obj
            //auto* enttobj = scene.AddEmpty(emptyCreated++);        
            //scene.createEntity(enttobj, path, false);
            Model m = Model(path);
            auto meshes = m.loadModel();

            for(int i = 0; i < meshes.size()/2; ++i)
            {
                auto* obj = scene.createObject("mesh" + std::to_string(i)); 
                obj->addModel(std::move(meshes[i]));
                //obj->getTransform().setScale({0.1,0.1,0.1});
            }
        }
        
        
        path.resize(100);
        ImGui::InputText("path", (char*)path.c_str(), 100);
        if (ImGui::Button("Load Prefab")) {
            scene.deserialize(path);  
        }

        ImGuiDragDropFlags src_flags = 0;
        src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
        src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
        //src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
                

        ImGui::LabelText("", "Game Objects in Scene.");

        if (ImGui::TreeNodeEx("Scene objects", parent_flags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);
            auto objects = scene.getObjects();
            for (int i = 0; i < objects.size(); i++)
            {
                if (!objects[i])
                    continue;

                //TODO(darius) selectalbes here? This chunk can be used to create selectable think
                /*
                ImGui::Selectable(objects[i]->getName().c_str());
                if (ImGui::BeginDragDropSource(src_flags))
                {
                    if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip)){

                    }

                    ImGui::SetDragDropPayload("DND_DEMO_NAME", &i, sizeof(int));
                    ImGui::EndDragDropSource();
                }
                */

                ImGuiTreeNodeFlags node_flags = objects[i]->getChildObjects().empty() ? leaf_flags : parent_flags;

                if (ImGui::TreeNodeEx(objects[i]->getName().c_str(), parent_flags)) {
					objects[i]->traverseObjects([&](Object* op) {
						ImGuiTreeNodeFlags node_flags = op->getChildObjects().empty() ? leaf_flags : parent_flags;
						if (ImGui::TreeNodeEx(op->getName().c_str(), parent_flags)) {
							bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
							if (ImGui::IsItemClicked()) {
								show_object_window = true;
                                //if(item_clicked == nullptr)
								item_clicked = op;

                                if(item_clicked->getRigidBody()){
                                    //objTr = item_clicked->getRigidBody()->tr.get_quatmat();
                                }
							}
							ImGui::TreePop();
						}
					});

					bool node_open2 = ImGui::TreeNodeEx("object", leaf_flags);
                    if (ImGui::IsItemClicked()) {
                        show_object_window = true;

						//if(item_clicked == nullptr)
						item_clicked = objects[i];
                    }

					ImGui::TreePop();
                }
            }
            ImGui::TreePop();
            ImGui::PopStyleVar();
        }
    }
    
    ImGui::End();

	//if (show_object_window)
    //{
        showObjectWindow(item_clicked, r, scene);
    //}

}

void UI::showObjectWindow(Object* obj, Renderer& r, Scene& scene)
{
    if (!obj) {
        //std::cout << "no obj provided. exsiting showObjectWindow(obj)\n";
        return;
    }

	ImGui::Begin("Object window", &show_object_window);
	//ImGui::Text(obj -> getName().c_str());
    obj->getName().resize(max_name_length);

    ImGui::InputText("Name: ", (char*)obj->getName().c_str(), obj->getName().size());
    if(ImGui::Button("clear name"))
        obj->getName().clear();
    
	ImGui::Checkbox("Hide Object", &obj -> objectHiddenState());

    //TODO(darius) so objTr updates quaternion at the end of UI work, cause of Guizmos. This is ugly

    auto tr = obj->getTransform();
    if (ImGui::CollapsingHeader("Transform component")){
       /* ImGui::Text("position %f, %f, %f", objTr[3][0], objTr[3][1], objTr[3][2]);
        ImGui::DragFloat("position x", &objTr[3][0], 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position y", &objTr[3][1], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position z", &objTr[3][2], 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        ImGui::Text("scale %f, %f, %f", tr.scale.x, tr.scale.y, tr.scale.z);
        ImGui::DragFloat("scale x", &tr.scale.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("scale y", &tr.scale.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("scale z", &tr.scale.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        */
        
        auto pos = item_clicked->getTransform().getPosition();

        ImGui::DragFloat("position x", &pos.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position y", &pos.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position z", &pos.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", 1);

        if(ImGui::Button("reset")){
            item_clicked->getTransform().resetRotation();
        }

    }

    if (ImGui::CollapsingHeader("DirectionalLight component")) {
        ImGui::DragFloat("position x", &DirectionalLight::lightPos.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position y", &DirectionalLight::lightPos.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position z", &DirectionalLight::lightPos.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

        ImGui::DragFloat("point x", &DirectionalLight::lightPoint.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("point y", &DirectionalLight::lightPoint.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("poiint z", &DirectionalLight::lightPoint.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

        ImGui::DragFloat("dir x", &DirectionalLight::direction.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("dir y", &DirectionalLight::direction.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("dir z", &DirectionalLight::direction.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

        ImGui::DragFloat("orto x", &DirectionalLight::orthoBounds.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orto y", &DirectionalLight::orthoBounds.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orto z", &DirectionalLight::orthoBounds.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("orto w", &DirectionalLight::orthoBounds.w, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
    }

    if (ImGui::CollapsingHeader("DirectionalLight component")) {
        ImGui::Checkbox("ShadowCaster", &obj->shadowCasterRef()); 
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
        ImGui::Checkbox("Static Object", &obj -> getRigidBody()->get_is_static_ref()); 
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

    auto& spriteAnimator = obj->getSpriteAnimator(); 

    if (spriteAnimator && ImGui::CollapsingHeader("Animator component")){
        ImGui::Indent( 16.0f );
        //Indenter(16.0f);

        if(ImGui::Button("init animator")) 
            obj->initAnimator();

        ImGui::Text("Animator animations count: %i", spriteAnimator->size());
        ImGui::Text("Animator currAnim number: %i", spriteAnimator->currPlayedAnim());

        if(ImGui::Button("Next Animation")){
            spriteAnimator->nextAnim();
        }

        if(ImGui::CollapsingHeader("Animations:"))
        {
            ImGui::Indent( 16.0f );

            auto& anims = spriteAnimator->getAnimations(); 
            for(size_t i = 0; i < anims.size(); ++i){
                auto& animI = anims[i];
                ImGui::Text("SpriteAnimation: %i",i);

                ImGui::DragFloat("delay", animI.getDelay(), 1.0f, 0, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("rows", animI.getRows(), 1.0f, 0, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("cols", animI.getCols(), 1.0f, 0, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("length", animI.getLength(), 1.0f, 0, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("start", animI.getStart(), 1.0f, 0, FLT_MAX, "%.3f", 1);
                ImGui::DragFloat("border", animI.getBorder(), 0.1f, 0, FLT_MAX, "%.1f", 1);
                ImGui::Checkbox("Play me", animI.getPlay());
                if(ImGui::Button("CropPoints")){
                    animI.initPoints();
                }
            }
            ImGui::Unindent( 16.0f );
        }

        if(ImGui::CollapsingHeader("Add Sprite Animation")){

            ImGui::InputText("path", (char*)path.c_str(), 100); 

            if(ImGui::Button("load")) 
                item_clicked->addSpriteAnimation(SpriteAnimation(path));
        }
        ImGui::Unindent( 16.0f );
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

        ImGui::Checkbox("active", collider->activeAdr());
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

        ImGui::DragFloat("color factor", &light->ambientFactor, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("diffuse factor", &light->diffuseFactor, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

        ImGui::DragFloat("specular x", &light->specular.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("specular y", &light->specular.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("specular z", &light->specular.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
    }

   

    auto& modelV = obj->getModel();
    if(modelV && ImGui::CollapsingHeader("ModelMesh"))
    {
        ImGui::Text("Draw Mode: %i", modelV->meshes[0].getDrawMode()); 

        ImGui::Text("How much meshes? %i", modelV->meshes.size()); 

        for(auto& m : modelV->meshes)
        {
            ImGui::Text("Mesh vertices size: %i", m.getVertices().size()); 
        }

        if(ImGui::Button("AABB"))
        {
            modelV->meshes[0].calculateAabb(obj->getTransform());
            assert(Renderer::currentRendererPtr);
            for(auto& mi : modelV->meshes)
            {
                auto aabb = mi.getAABB();
                Renderer::currentRendererPtr->getDebugRenderer().aabbToRender.push_back(std::move(aabb));//(globalCenter, scl.x, scl.y, scl.z);
            }
        }
    }

    auto& model = obj->getModel();
    if (model && ImGui::Button("batch model"))
    {
        model->meshes[0].addVerticesBath(model->meshes[0], obj->getTransform().getPosition());

        Mesh& batchRef = model->meshes[0];

        Mesh batchCopy(batchRef.getVertices(), batchRef.getIndices(), batchRef.getTextures());

        model->meshes.clear();

        model->addMesh(batchCopy);
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

    //guizmoWindow();

	ImGui::End();
}

void UI::objectManipulationWindow(Scene& scene)
{
    if(!item_clicked)
        return;
    ImGui::Begin("Objects manipulation");

    if (ImGui::Button("Show Object") && item_clicked) {
        auto v = item_clicked->getTransform().getPosition();
        v += glm::vec3{0, 0, 10};

        glm::vec3 front = glm::normalize(item_clicked->getTransform().getPosition() - v);

        GameState::instance->cam.setCameraPos(v);
        GameState::instance->cam.setCameraFront(front);
        GameState::instance->cam.setUnexpectedUpdate(true);
    }

    if (ImGui::Button("Copy Object") && item_clicked) {
        Object tmp = *item_clicked;
        item_copy = scene.createObject(tmp);
    }

    if (ImGui::Button("Delete Object") && item_clicked) {
        //ImGui::Text("not implemented yet");
        scene.destroyObject(item_clicked->getName());
        item_clicked = nullptr;
    } 

    if (ImGui::Button("Make prefab") && item_clicked) {
        std::string prefabName = item_clicked->getName();

        //NOTE(darius)TODO(darius) because we resize name name etter Ui element. We cant concat stirngs here.Fix it
        //prefabName.substr(0, max_name_length-1);

        //prefabName.shrink_to_fit();
        int c = 0;
        for (int i = 0; prefabName[i]; i++) {
            c++;
        }

        prefabName.resize(c);

        prefabName.append(".prefab");

        std::string sum = GameState::engine_path + prefabName;
        scene.serializePrefab(item_clicked, sum);
    }

    ImGui::End();
}

void UI::componentAdderWindow(Renderer& hui)
{
    if(!show_component_adder || !item_clicked)
        return;

    //ImGui::Begin("Add Component");

    if(ImGui::Button("Collider"))
        item_clicked->addCollider();

    if (ImGui::Button("RigidBody"))
        item_clicked->addRigidBody();

    if(item_clicked->getModel() && ImGui::Button("PointLight")){
        item_clicked->addPointLight();    
    }

    if(item_clicked->getModel() && ImGui::CollapsingHeader("ParticleSystem")){
        path.reserve(100);
        if(ImGui::Button("pathset"))
            path = GameState::engine_path + "textures/birdParticle1.png";
        ImGui::InputText("path", (char*)path.c_str(), 100); 
        
        if(ImGui::Button("Load")){
            FlatMesh flat;
            flat.setTexture(path); 
            ParticleSystem ps = ParticleSystem();
            ps.addParticle(std::move(flat));

            item_clicked->addParticleSystem(std::move(ps));    
        }
    }

    if(item_clicked->getModel() && ImGui::Button("Material"))
    {
        Material m;

        m.ambient = {1,0,1};
        m.diffuse = {1,0,1};
        m.specular = {1,0,1};
        m.shininess = 32;

        item_clicked->setMaterial(m);
    }

    if(ImGui::CollapsingHeader("Model")){
        ImGui::Indent();

        size_t routine = hui.getShaderRoutine();
        Shader shader = hui.getShader();

        if(ImGui::CollapsingHeader("ModelFile")){
            ImGui::Indent();

            path.resize(100);
            ImGui::InputText("path", (char*)path.c_str(), 100);

            if(ImGui::Button("Load")){
                item_clicked->addModel(path);
            }

            ImGui::Unindent();
        }

        if(ImGui::CollapsingHeader("Animated ModelFile")){
            ImGui::Indent();

            path.resize(100);
            ImGui::InputText("path", (char*)path.c_str(), 100);

            if(ImGui::Button("Load")){
                path.shrink_to_fit();
                std::string animPath = "meshes/animations/stupidrun.DAE";

                item_clicked->addModel(GameState::engine_path + animPath);

                /*print("Model bones:\n");
                auto& bones = item_clicked->getModel().value().GetBoneInfoMap();
                for(auto& b : bones)
                {
                    print(b.second.offset, "\n-------------\n");
                }*/

                SkeletalAnimation* danceAnimation = new SkeletalAnimation(GameState::engine_path + animPath, &item_clicked->getModel().value());
                
                /*print("Animation:\n"); 
                std::cout << *danceAnimation;
                */

                item_clicked->setSkeletalAnimation(danceAnimation);

                //delete danceAnimation;
            }

            ImGui::Unindent();
        }
            
        if(ImGui::CollapsingHeader("Cube")){
            ImGui::InputText("path", (char*)path.c_str(), 100);

            path = GameState::engine_path + "textures/checkerboard.png";

            if(ImGui::Button("Load CubeMesh")){
                CubeMesh cube;

                path.shrink_to_fit();

                cube.setTexture(path);

                item_clicked->addModel(std::move(cube));
            }
        }

        if(ImGui::CollapsingHeader("FlatMesh"))
        {
            path.resize(100);
            name.resize(100);
            ImGui::InputText("path", (char*)path.c_str(), 100);
            //ImGui::InputText("name", (char*)name.c_str(), 100);

            if(ImGui::Button("Load FlatMesh")){
                FlatMesh flat;

                path.shrink_to_fit();
                //name.shrink_to_fit();

                flat.setTexture(path);

                item_clicked->addModel(std::move(flat));
            }

            if(ImGui::Button("Load Normal Mesh")){
                path.shrink_to_fit();
                name.shrink_to_fit();
                //TODO(darius) fix this cringe
                FlatMesh* itemFlat = (FlatMesh*)&(item_clicked->getModel()->meshes[0]);
                (itemFlat)->setTextureNormal(path);
            }

            if(ImGui::Button("Load Parallax")){
                path.shrink_to_fit();
                name.shrink_to_fit();
                //TODO(darius) fix this cringe
                FlatMesh* itemFlat = (FlatMesh*)&(item_clicked->getModel()->meshes[0]);
                (itemFlat)->setTextureParallax(path);
            }

            if(!item_clicked -> getModel() && ImGui::Button("Load SpriteAnimation")){
                FlatMesh flat;

                path.shrink_to_fit();
                name.shrink_to_fit();

                flat.setTexture(path, name);

                item_clicked->addModel(std::move(flat));

                item_clicked->addSpriteAnimation(SpriteAnimation(4,8,500));
            }

			if(!item_clicked -> getModel() && ImGui::Button("Load SpriteAnimation From Folder")){
                path.shrink_to_fit();

                FlatMesh flat;

                item_clicked->addModel(std::move(flat));
                item_clicked->addSpriteAnimation(SpriteAnimation(path));
            }
        }

        if(ImGui::Button("Empty")){
            item_clicked->addModel();
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Script"))
    {
        path.resize(100);
        path = GameState::engine_path + "logicScripts/EngineLogic/x64/Debug";
		ImGui::InputText("path", (char*)path.c_str(), 100);
        if (ImGui::Button("Load Script")) 
        {
            path.shrink_to_fit();
            //DANGER(darius) leak
            //TODO(darius) make factories
            EmptyScriptRoutine* routine = new EmptyScriptRoutine(path.c_str(), GameState::instance);
            //Script s(hui.getScene(), item_clicked, routine);
            item_clicked->addScript(hui.getScene(), routine);
            item_clicked->getScript()->startScript();
        }
    }

    if(ImGui::Button("Camera")){
        hui.getScene()->addCameraToScene(hui.getScene()->createCamera());

    }


    /*if(item_cicked->getModel() && ImGui::Button("Animation")){
        Animation* danceAnimation = new Animation("../../../meshes/animations/bot/bot.dae", &item_cicked->getModel().value());
        item_cicked->setAnimator(danceAnimation);
    }
    */

    //ImGui::End();
} 

void UI::guizmoWindow()
{
    if(!item_clicked){
        return;
    }

    ImGui::Begin("Guizmos", &show_object_window);

    if (ImGui::Button("Translation tool"))
    {
        guizmoTool.setOperation(ImGuizmo::OPERATION::TRANSLATE);
    }

    if (ImGui::Button("Rotation tool"))
    {
        guizmoTool.setOperation(ImGuizmo::OPERATION::ROTATE);
    }

    if (ImGui::Button("Scale tool"))
    {
        guizmoTool.setOperation(ImGuizmo::OPERATION::SCALE);
    }

    if (ImGui::Button("Change Mode"))
    {
        if(guizmoTool.getMode() == ImGuizmo::LOCAL)
            guizmoTool.setMode(ImGuizmo::WORLD);
        else
            guizmoTool.setMode(ImGuizmo::LOCAL);
    }

    guizmoTool.tool(item_clicked);

	//float viewManipulateRight = ImGui::GetWindowPos().x + 900.0f;
	//float viewManipulateTop = ImGui::GetWindowPos().y;
    //ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(glm::mat4(1)), 100.f);
	//ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), 8, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

    ImGui::End();
}

void UI::showConsoleWindow()
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

void UI::showEditorSettingsWindow(Renderer& hui)
{
    ImGui::Begin("Editor Settings");

    ImGui::DragFloat("camera speed", GameState::instance->cam.getCameraSpeed(), 0.1f, 0, 100, "%.3f", 1);

    ImGui::Checkbox("Enable Frustum Culling", &GameState::cullEnabled);
    ImGui::Checkbox("Enable Shadows", &GameState::shadowEnabled);

    ImGui::DragFloat("gamma brightness", &GameState::gammaBrightness, 0.005f, 0, 10, "%.0001f", 1);

    //ImGui::DragFloat("gamma factor", &GameState::gammaFactor, 0.01f, 0, 10, "%.001f", 1);
    
    ImGui::ColorEdit3("Background Color", (float*)&hui.backgroundColor);

    if (SystemInfo::getInfo()) {
        ImGui::Text("CPU INFO: ");
        ImGui::Text(SystemInfo::getInfo()->getCPU().data());

        ImGui::Text("GPU INFO: ");
        ImGui::Text(SystemInfo::getInfo()->getGPU().data());
        if(SystemInfo::getInfo()->isOcclusionCullingAvailable())
            ImGui::Text("Occlusion Culling is Available\n");

        ImGui::Text("MEMORY INFO: ");
        auto memoryInformation = SystemInfo::getInfo()->getMemoryInfo(); 
        ImGui::Text("total virtual memory amount     : %lld Mb", memoryInformation.totalVirtualMem/1024/1024);
        ImGui::Text("total virtual memory used       : %lld Mb", memoryInformation.virtualMemUsed/1024/1024);
        ImGui::Text("total phys memory used          : %lld Mb", memoryInformation.physMemUsed/1024/1024);
        ImGui::Text("total virtual memory used by me : %lld Mb", memoryInformation.virtualMemUsedByMe/1024/1024);
        ImGui::Text("total phys memory used by me    : %lld Mb", memoryInformation.physMemUsedByMe/1024/1024);
    }

    ImGui::Text("Frame Tick: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(timeVal).c_str());

    profile.addRecord(std::round(1 / timeVal));

    if(profile.size() > 0)
        ImGui::PlotLines("Frame Times", &profile.getHistoryRef()[0], profile.getHistoryRef().size(), 0, NULL, 0, 500, ImVec2(0,100));

    if (ImGui::Button("Serialize profiler")){
        profile.serialize(GameState::engine_path + profile.getNameRef() + ".profile");
    }

    ImGui::Text("Frame Rate: ");
    ImGui::SameLine();
    long rate = std::round(1 / timeVal);
    ImGui::Text(std::to_string(rate).c_str());

    ImGui::Text("Draw Calls Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(Renderer::drawCallsCount).c_str());

    ImGui::Text("Instanced Draw Calls Count: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(Renderer::drawCallsInstancedCount).c_str());

    ImGui::End();
}

void UI::profilesWindow(std::vector<Profiler<float>>& profilers)
{
    ImGui::Begin("Profilers");

    for(auto& p : profilers)
    {
        if(p.size() > 0)
            ImGui::PlotLines(p.getNameRef().c_str(), &p.getHistoryRef()[0], p.getHistoryRef().size(), 0, NULL, 0, 1, ImVec2(0,100));
    }

    ImGui::End();
}

void UI::sceneCamerasWindow(Scene& scene)
{
    ImGui::Begin("Scene Cameras");

    if(ImGui::CollapsingHeader("EditorCamera")){
        auto& vec = GameState::instance->cam.getCameraPosRef();
        ImGui::DragFloat("position X", &vec.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position Y", &vec.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
        ImGui::DragFloat("position Z", &vec.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
    }

    int c = 0;

    for(auto* i : scene.getSceneCameras())
    {
        if(ImGui::CollapsingHeader(std::to_string(c++).c_str())){
            auto& vec = i->getCameraPosRef();
            ImGui::DragFloat("position X", &vec.x, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position Y", &vec.y, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);
            ImGui::DragFloat("position Z", &vec.z, 0.05f, -FLT_MAX, FLT_MAX, "%.3f", 1);

            if (ImGui::Button("Set As Main Camera")){
                //GameState::saveActiveCameraStateIntoEditorCamera();
                //GameState::cam = *i; 
                i->setAsActiveCamera();
            }
        }
    }

    if (ImGui::Button("Use Editor Camera")){
        GameState::setEditorCameraMode();
    }

    ImGui::End();
}

Object* UI::getItemClicked() 
{
    return item_clicked;
}

void UI::setItemClicked(Object* obj) 
{
    if(ImGuizmo::IsOver() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        return;
    item_clicked = obj;
}

void UI::setItemClickedForced(Object* obj) 
{
    item_clicked = obj;
}

void UI::setTime(double time)
{
    timeVal = time;
}
