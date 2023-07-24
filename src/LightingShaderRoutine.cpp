#include <LightingShaderRoutine.h>
#include <iostream>

void LightingShaderRoutine::operator() (Transform tr){
    glUseProgram(sv.getProgram());
	sv.setVec3("viewPos", GameState::cam.getCameraPos());
    sv.setInt("lightsCount", PointLight::LightsCount);
    sv.setFloat("gammaFactor", 1); 

    glm::mat4 projection = GameState::cam.getPerspective(1600, 900);
    glm::mat4 view = (GameState::cam.getBasicLook());

    sv.setMat4("projection", projection);
    sv.setMat4("view", view);

    //if(directionalLight)
    //    directionalLight->setShaderLight(sv);
    //if(pointLight)
    //    pointLight->setShaderLight(sv);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = tr.position;
    glm::mat4 q = tr.get_quatmat();
    glm::vec3 scale = tr.scale;

    model = glm::translate(model, pos);
    model = glm::scale(model, scale);
    model *= q;

    sv.setMat4("model", model);
}
