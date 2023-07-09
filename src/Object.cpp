#include <Object.h>


Object::Object(std::string name_in) : name(name_in)
{
    material = std::nullopt;
}

Object::Object(std::string name_in, Shader model_shader, LightingShaderRoutine& shaderRoutine_in)
{
    model.emplace("", model_shader, shaderRoutine_in, false, false);

    tr.position = {0,0,0};
    tr.scale = {0,0,0};
    name = name_in;
}

Object::Object(std::string name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, std::string_view model_path_in, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
                                                                                                Scene* scn, EmptyScriptRoutine* routine,
                                                                                                bool gammaShader, bool rotateTextures)
{
    model.emplace(model_path_in, model_shader, shaderRoutine_in, gammaShader, rotateTextures);
    
    if(model.has_value())
        model.value().loadModel();

    script = Script(scn, this, routine);

    tr.position = pos_in;
    tr.scale = scale_in;

    rbody.emplace(RigidBody(0.1, tr, false));
    rbody.value().create_box_inertia_tensor(10, { 1,1,1 });

    name = name_in;

    colider.emplace(collider_in, tr);
}

Object::Object(Object* parentObject, Mesh& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in)
{
    parent = parentObject;
    model.emplace(m, model_shader, shaderRoutine_in);

    tr = parentObject->getTransform();//TODO(darius) fix it

    rbody.emplace(1,tr,false);
    rbody.value().get_is_static_ref() = true;

    //apply_force({0.1,0.1,0.1});

    name = parentObject->get_name() + " child " + std::to_string((size_t)this);

    colider.emplace(parentObject -> getColider()->get_size(), tr, 0, false);

    script = parentObject->getScript();
    script -> setParentObject(this);
}

Object::Object(std::string&& name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, Mesh& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
                                                                                Scene* scn, EmptyScriptRoutine* routine, bool active)
{
    name = std::move(name_in);
    tr = Transform(pos_in, scale_in);
    rbody.emplace(0.1, tr, false);
    rbody.value().get_is_static_ref() = true;
    model.emplace(m, model_shader, shaderRoutine_in);
    script = Script(scn, this, routine);
    colider.emplace(collider_in, tr, 0, active);
}

Object::Object(std::string&& name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, Model& m, Shader model_shader, LightingShaderRoutine& shaderRoutine_in,
                                                                                Scene* scn, EmptyScriptRoutine* routine, bool active)
{
    name = std::move(name_in);
    tr = Transform(pos_in, scale_in);
    rbody.emplace(0.1, tr, false);
    rbody.value().get_is_static_ref() = true;
    model.emplace(m);//Model(m, model_shader, shaderRoutine_in);
    script = Script(scn, this, routine);
    colider.emplace(collider_in, tr, 0, active);
}

void Object::setupScript(EmptyScriptRoutine* r)
{
    script.value().set_scripts(r);
}

void Object::startScript()
{
    if (!script.has_value())
        return;
    script.value().startScript();
    //traverseChilds([](Object* op) {op->startScript(); });
}

void Object::updateScript() 
{
    if(script.has_value())
        script.value().updateScript();
    //traverseObjects([](Object* op){op->updateScript()});
}

void Object::updateAnimator(float dt)
{
    if(!animator)
        return;
    animator->UpdateAnimation(dt);
}

void Object::renderObject() 
{
    if(!object_hidden && model){
        model.value().Draw(Transform(getTransform()), getPointLight(), getMaterial());
    }

    //DANGER! -> traverseObjects([](Object* op) {op->renderObject(); });

    traverseChilds([](Object* op) {op->renderObject(); });
}

void Object::updateParticleSystem(float dt)
{
    return;
    if(particles){
        particles->updateUniform3DDistribution(dt);
        particles->renderParticles();
    }
}

void Object::apply_force(glm::vec3 force) 
{
    if(!rbody)
        return;
    rbody.value().add_force(force);
}

void Object::updatePos() 
{
    if(rbody.has_value())
        rbody.value().update(0.01);

    //DANGER! -> traverseObjects([](Object* op) {op->updatePos(); });
    traverseChilds([](Object* op) {op->updatePos(); });
}

glm::vec3 Object::get_pos() 
{
    //if (parent.has_value())
    //	return parent.value()->get_pos();
    return getTransform().position;
}

//TODO(darius) fuck you, incapsulation
glm::vec3& Object::get_pos_ref()
{
    if (parent)
        return parent->get_pos_ref();
    return getTransform().position;
}

std::optional<Colider>& Object::getColider() 
{
    return colider;
}

std::optional<RigidBody>& Object::getRigidBody()
{
    return rbody;
}

std::optional<Model>& Object::getModel()
{
    return model;
}

void Object::frozeObject()
{
    rbody.value().is_static = true;
}

void Object::unfrozeObject()
{
    rbody.value().is_static = false;
}

Transform& Object::getTransform()
{
    //if (parent.has_value())
    //	return parent.value()->getTransform(); 
    return tr;
}

Transform& Object::getParentTransform()
{
    return tr;
}	

std::optional<Script>& Object::getScript()
{
    return script;
}

void Object::set_child_objects(std::vector<Object*>&& objects)
{
    child_opbjects = objects;
}

std::vector<Object*>& Object::get_child_objects() 
{
    return child_opbjects;
}

void Object::addChild(Object* obj)
{
    child_opbjects.push_back(obj);
}

void Object::traverseChilds(std::function<void(Object*)> functor)
{
    for (auto& i : child_opbjects) {
        //std::cout << "tr " << tr.position.x << "|" << tr.position.y << "|" << tr.position.z << "\n";
        //std::cout << "child x: " << i->getTransform().position.x << "\n";
        functor(i);
    }
}

void Object::traverseObjects(std::function<void(Object*)> functor, bool include_root)
{
    std::queue<Object*> q;
    if(include_root)
        q.push(this);

    for (auto& i : child_opbjects)
        q.push(i);

    while(!q.empty())
    {
        auto curr = q.front();
        q.pop();

        functor(curr);

        for (auto& i : curr -> get_child_objects()) {
            q.push(i);
        }
    }
}

std::string& Object::get_name() 
{
    return name;
}

void Object::hide()
{
    object_hidden = true;
}

void Object::unhide()
{
    object_hidden = false;
}

bool& Object::object_hidden_state()
{
    return object_hidden;
}

void Object::addPointLight(PointLight&& pl, glm::vec3 pos)
{
    if(pointLight || !model)
        return;
    pointLight = pl;
    pointLight->addLight();
    //pointLight.position = pos;
}

void Object::addCollider()
{
    if(colider)
        return;
    colider.emplace(glm::vec3{0,0,0}, tr);
}

void Object::addModel(Mesh m, Shader sv, LightingShaderRoutine routine)
{
    if(model)
        return;
    //setDefaultMaterial();
    model.emplace(m, sv, routine);
}

void Object::addModel(Shader sv, LightingShaderRoutine routine)
{
    if(model)
        return;
    //setDefaultMaterial();
    model.emplace(sv, routine);
}

//TODO(darius) when loading big models add threading
void Object::addModel(std::string_view path, Shader sv, LightingShaderRoutine routine, bool rotate)
{
    if(model)
        return;
    //setDefaultMaterial();
    model.emplace(path, sv, routine, rotate);
}

void Object::addModel(std::string_view path)
{
    if(model)
        return;
    //setDefaultMaterial();
    model.emplace(path);
}

void Object::addParticleSystem(ParticleSystem&& ps)
{
    if(particles)
        return;

    particles = ps;	
    //particles->setEmitter(emitter);
}

std::optional<ParticleSystem>& Object::getParticleSystem()
{
    return particles;	
}

std::optional<PointLight>& Object::getPointLight()
{
    return pointLight;
}

void Object::setDefaultMaterial()
{
    setMaterial(Material(32));
}

void Object::setMaterial(const Material& m)
{
    if(material)
        return;
    material = m;
}

std::optional<Material>& Object::getMaterial()
{
    return material;
}

void Object::setAnimator(Animation* anim)
{
    if(animator)
        return;
    animator.emplace(anim);
}

std::optional<Animator>& Object::getAnimator()
{
    return animator;
}


