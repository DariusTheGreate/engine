#include <LightingShaderRoutine.h>

void LightingShaderRoutine::operator() (Transform tr){
	sv.setVec3("viewPos", GameState::cam.getCameraPos());
    sv.setInt("lightsCount", PointLight::LightsCount);
    sv.setFloat("gammaFactor", 1); 

    if(directionalLight)
        directionalLight->setShaderLight(sv);
    if(pointLight)
        pointLight->setShaderLight(sv);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = tr.position;
    glm::mat4 q = tr.get_quatmat();
    glm::vec3 scale = tr.scale;

    model = glm::translate(model, pos);
    model *= q;
    model = glm::scale(model, scale);

    sv.setMat4("model", model);
}
