#include <LightingShaderRoutine.h>
#include <Renderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void LightingShaderRoutine::operator() (Transform tr){
    //glUseProgram(sv.getProgram());
    sv = Renderer::shaderLibInstance->getCurrShader();
    glUseProgram(sv.getProgram());
	sv.setVec3("viewPos", GameState::cam.getCameraPos());
    sv.setInt("lightsCount", PointLight::LightsCount);
    sv.setFloat("gammaFactor", 1); 

    /*glm::vec3 lightPos = glm::vec3(-2.0f, 0.0f, -1.0f);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    */

    sv.setMat4("lightSpaceMatrix", DirectionalLight::getLightMat());

    glm::mat4 projection = GameState::cam.getPerspective(1600, 900);
    glm::mat4 view = (GameState::cam.getBasicLook());

    sv.setVec3("lightPos", DirectionalLight::lightPos);
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

    /*if (Renderer::shaderLibInstance->stage == ShaderLibrary::STAGE::SHADOWS)
    {
        std::cout << "suka\n";
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Renderer::shaderLibInstance->depthMap);
    }
    */
}
