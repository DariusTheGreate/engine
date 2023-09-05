#include <LightingShaderRoutine.h>
#include <Renderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void LightingShaderRoutine::operator() (Object* obj){
    //glUseProgram(sv.getProgram());
    sv = Renderer::shaderLibInstance->getCurrShader();
    glUseProgram(sv.getProgram());

	sv.setVec3("viewPos", GameState::cam.getCameraPos());
    sv.setInt("lightsCount", PointLight::LightsCount);
    sv.setFloat("gammaFactor", GameState::gammaFactor); 
    sv.setFloat("gammaBrightness", GameState::gammaBrightness); 

    //std::cout << "ID: " << obj->getID() << "\n";
    /*int id = obj->getID() * 25500;
    double r = ((id) & 0x000000FF) >> 0;
    double b = ((id) & 0x0000FF00) >> 8;
    double g = (id & 0x00FF0000) >> 16;

    sv.setVec4("PickingColor", glm::vec4{float(r/255),float(g/255),float(b/255),0});
    */

    double id = obj->getID();

    sv.setVec4("PickingColor", glm::vec4{id/255,id/255,id/255,0});

    /*glm::vec3 lightPos = glm::vec3(-2.0f, 0.0f, -1.0f);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    */

    sv.setMat4("lightSpaceMatrix", DirectionalLight::getLightMat());

    glm::mat4 projection = GameState::cam.getPerspective(1920, 1080);
    glm::mat4 view = (GameState::cam.getBasicLook());

    sv.setVec3("lightPos", DirectionalLight::lightPos);
    sv.setMat4("projection", projection);
    sv.setMat4("view", view);

    //if(directionalLight)
    //    directionalLight->setShaderLight(sv);
    //if(pointLight)
    //    pointLight->setShaderLight(sv);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = obj->getTransform().getPosition();
    glm::mat4 q = obj->getTransform().matrix;
    //glm::vec3 scale = obj->getTransform().geS;


    //TODO(darius) add distanced rendering here. NOTE(darius) Not that hard
    float dist = glm::length(GameState::cam.getCameraPos() - pos);
    if(dist > 99) 
    {
        //scale *= 0.5;//2 * GameState::cam.getFov()/dist;
        pos = GameState::cam.getCameraPos() + ((GameState::cam.getCameraPos() - pos));
    }

    //model = glm::translate(model, pos);
    //model = glm::scale(model, scale);
    //model *= q;

    //NOTE(darius) looks like bug with guiamo rotation comes form here..
    model = glm::mat4(1.0f);
    model *= obj->getTransform().matrix;
    glm::vec3 posm = obj->getTransform().getPosition();
    //glm::mat4 mvp = glm::translate(model, posm);

    //model[3] = glm::vec4(posm.x, posm.y, posm.z, 0);
    //model = glm::translate(model, obj->getTransform().getPosition());
    //model = glm::scale(model, obj->getTransform().getScale());
    //glm::quat qu = obj->getTransform().matrix;
    //glm::mat4 RotationMatrix = glm::toMat4(qu);
    //NOTE(darius) brokes if uncomment
    //model *= RotationMatrix;

    sv.setMat4("model", model);

    /*if (Renderer::shaderLibInstance->stage == ShaderLibrary::STAGE::SHADOWS)
    {
        std::cout << "suka\n";
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Renderer::shaderLibInstance->depthMap);
    }
    */
}
