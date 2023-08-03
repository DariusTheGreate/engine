#include <Object.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cctype>

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

    rbody.emplace(RigidBody(0.1f, tr, false));
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

Object::Object(std::string&& name_in, glm::vec3 pos_in, glm::vec3 scale_in, glm::vec3 collider_in, const Model& m, Scene* scn, EmptyScriptRoutine* routine, bool active)
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
        model.value().Draw(this, getPointLight(), getMaterial());
    }

    //DANGER! -> traverseObjects([](Object* op) {op->renderObject(); });

    traverseChilds([](Object* op) {op->renderObject(); });
}

void Object::updateParticleSystem(float dt)
{
    if(particles){
        particles->updateUniform3DDistribution(dt);
        particles->renderParticles();
    }
}

void Object::updateSpriteAnimation(float dt) 
{
    if (spriteAnimation) {
        spriteAnimation->update(dt);
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
    if (colider && *colider.value().get_collision_state() == true)
        return;

    if(rbody.has_value())
        rbody.value().update(0.01f);

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

void Object::addRigidBody(float mass)
{
    rbody.emplace(mass, tr, true);
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

void Object::moveTransform(glm::vec3 v)
{
    if (colider && *colider.value().get_collision_state() == true)
        return;

    getTransform().position += v;
}

void Object::addScript(Scene* scn, EmptyScriptRoutine* routine)
{
    if (script.has_value())
        return;
    script = Script(scn, this, routine);
    //NOTE(darius) Should I start it here?
    //script->startScript();
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

void Object::addCollider(glm::vec3 size, glm::vec3 shift)
{
    if(colider)
        return;
    Colider col(size, tr);
    col.set_shift(shift);
    colider.emplace(std::move(col));
}

void Object::addModel(Mesh&& m, Shader sv, LightingShaderRoutine routine)
{
    if(model)
        return;
    //setDefaultMaterial();
    model.emplace(std::move(m), sv, routine);
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

    particles.emplace(std::move(ps));	
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

void Object::setID(int id)
{
    ID = id;
}

int Object::getID()
{
    return ID;
}

void Object::addSpriteAnimation(SpriteAnimation&& anim)
{
    if(spriteAnimation)
        return;
    spriteAnimation.emplace(std::move(anim));
    if (model && model->meshes.size() > 0)
        spriteAnimation->setSprite((FlatMesh*) &model->meshes[0]);
}

//todo(darius) check NRVO
SpriteAnimation Object::excnahgeSpriteAnimation(SpriteAnimation&& anim)
{
    assert(spriteAnimation);

    SpriteAnimation&& tmp = std::move(spriteAnimation.value());
    spriteAnimation.emplace(std::move(anim));

    return tmp;
}

void Object::setSpriteAnimation(SpriteAnimation& anim)
{
    spriteAnimation.emplace(anim);

    if (model && model->meshes.size() > 0)
        spriteAnimation->setSprite((FlatMesh*)&model->meshes[0]);
}

std::optional<SpriteAnimation>& Object::getSpriteAnimation()
{
    return spriteAnimation;
}


void Object::serialize(std::ofstream& file)
{
    file << "Object: {\n";
    file << "\tName: {\n\t\t" << get_name().c_str() << "\n\t}\n";
    file << "\tHidden: {\n\t\t" << (object_hidden == true ? "true" : "false") << "\n\t}\n";
    //CM std::to_string() to convert digit to string
    {
        file << "\tTransform: {\n";
        file << "\t\tPosition: {" << std::to_string(tr.position.x) << " " << std::to_string(tr.position.y) << " " << std::to_string(tr.position.z) << "}\n";
        file << "\t\tScale: {" << std::to_string(tr.scale.x) << " " << std::to_string(tr.scale.y) << " " << std::to_string(tr.scale.z) << "}\n";
        file << "\t}\n";
    }

    if (rbody)
    {
        file << "\tRigidBody: {" << std::to_string(rbody->mass) << " " << std::to_string(rbody->is_static) << " " << 0 << "}\n";
    }

    if (colider)
    {
        file << "\tCollider: {\n";
        file << "\t\tSize: {" << std::to_string(colider->get_size().x) << " " << std::to_string(colider->get_size().y) << " " << std::to_string(colider->get_size().z) << "}\n";
        file << "\t\tShift: {" << std::to_string(colider->get_render_shift().x) << " " << std::to_string(colider->get_render_shift().y) << " " << std::to_string(colider->get_render_shift().z) << "}\n";
        file << "\t}\n";
    }

    if (model)
    {
        //NOTE(darius) just store name of model. Name of shader. Wheather or not it has routine
        //NOTE(darius) in case it was created without path - store its meshes 
        //NOTE(darius) mesh seriliztion is just 1) storing vector of Vertices
                                              //2) indices
                                              //3) textures, which is just storing path => FlatMesh.setexture()

        file << "\tModel: {\n";
        model->path.shrink_to_fit();
        file << "\t\tPath: {" << model->path << "}\n";

        //NOTE(darius) basicly its a start for implementation of my own .obj file. But its slow asfuck and i wont get to it now. So here is boundary for object that I DO serialize -  it needs to be 1 mesh
        if (model->meshes.size() < 2) {
            file << "\t\tMeshes: {\n";

            for (auto& i : model->meshes)
            {
                //TODO(darius) optim copy
                auto vertices = i.getVertices();
                auto indices = i.getIndices();
                auto textures = i.getTextures();


                file << "\t\t\tVertices: {\n";
                for (auto& v : vertices)
                {
                    file << "\t\t\t\tPosition: {" << std::to_string(v.Position.x) << " " << std::to_string(v.Position.y) << " " << std::to_string(v.Position.z) << "}\n";
                    file << "\t\t\t\tNormal: {" << std::to_string(v.Normal.x) << " " << std::to_string(v.Normal.y) << " " << std::to_string(v.Normal.z) << "}\n";
                    file << "\t\t\t\tTexCoords: {" << std::to_string(v.TexCoords.x) << " " << std::to_string(v.TexCoords.y) << " " << std::to_string(0) << "}\n";
                    file << "\n";
                }
                file << "\t\t\t}\n";

                file << "\t\t\tIndices: {\n";
                for (auto& i : indices)
                {
                    file << "\t\t\t\tIndice: {" << i << "}\n";
                }
                file << "\t\t\t}\n";

                file << "\t\t\tTextures: {\n";
                for (auto& t : textures)
                {
                    file << "\t\t\t\tTexture: {";// << t.get_path() << "}\n";
                    
                    std::string tkn = t.get_path(); 
                    std::cout << tkn << "\n";
                    for(auto c : tkn)
                        if(c > -1 && c < 255 && c != '\0' && !std::isspace(c))
                            file << c; 
                    file << "}\n";

                    file << "\t\t\t\tType: {" << t.get_type() << "}\n";
                }

                file << "\t\t\t}\n";
            }

            file << "\t\t}\n";
        }
        file << "\t}\n";
    }

    if (spriteAnimation)
    {
        file << "\tSpriteAnimation: {\n";

        for (auto& p : spriteAnimation->getPoints()) 
        {
            file << "\t\t Point: {" << std::to_string(p.x) << " " << std::to_string(p.y) << " " << std::to_string(p.z) << " " << std::to_string(p.w) << "}\n";
            file << "\n";
        }

        file << "\t}\n";
    }

    if(script)
    {
        file << "\tScript: {\n";
        file << "\t\t Routine: {" << script->getRoutine()->getPath() << "}\n";
        file << "\t}\n";
    }

    file << "}\n";
}

