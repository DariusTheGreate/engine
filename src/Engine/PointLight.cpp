#include <Engine/PointLight.h>
#include <Rendering/Shader.h>

PointLight::PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in, float radius_in) : position(position_in), color(color_in), intensity(intensity_in), radius(radius_in) {
    //lightNumber = PointLight::LightsCount++;
}

void PointLight::addLight()
{
    lightNumber = PointLight::LightsCount++;
}

void PointLight::setShaderLight(const Shader& sv)
{
    diffuse = color * glm::vec3(diffuseFactor);
    ambient = diffuse * glm::vec3(ambientFactor);

    sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].position", position);
    sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].ambient", ambient);
    sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].diffuse", diffuse);
    sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].specular", specular);

    sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].constant", constant);
    sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].linear", linear);
    sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].quadratic", quadratic);
}

DirectionalLight::DirectionalLight(glm::vec3 direction_in, glm::vec3 color_in, float intensity_in, float radius_in) : color(color_in), intensity(intensity_in), radius(radius_in) {
    diffuse = color * glm::vec3(0.5f);
    ambient = diffuse * glm::vec3(0.2f);
}

void DirectionalLight::setShaderLight(const Shader& sv)
{
    sv.setVec3("dirLight.direction", direction);
    sv.setVec3("dirLight.ambient", ambient);
    sv.setVec3("dirLight.diffuse", diffuse);
    sv.setVec3("dirLight.specular", specular);
}

SpotLight::SpotLight(glm::vec3 position_in, glm::vec3 direction_in, glm::vec3 color, float radius_in) : position(position_in), direction(direction_in), radius(radius_in) {
    diffuse = color * glm::vec3(0.5f);
    ambient = diffuse * glm::vec3(0.2f);
}

void SpotLight::setShaderLight(const Shader& sv)
{
    sv.setVec3("light.position", position);
    sv.setVec3("light.ambient", ambient);
    sv.setVec3("light.diffuse", diffuse);
    sv.setVec3("light.specular", specular);

    sv.setFloat("light.constant", constant);
    sv.setFloat("light.linear", linear);
    sv.setFloat("light.quadratic", quadratic);
}

glm::mat4 DirectionalLight::getLightMat() 
{
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(orthoBounds.x, orthoBounds.y, orthoBounds.z, orthoBounds.w, near_plane, far_plane);
    //lightPos =// glm::vec3{0,1,0} * glm::vec3{0.1,0.1,0.1};
    lightView = glm::lookAt(lightPos, lightPoint, direction);
    lightSpaceMatrix = lightView;
    //println(lightSpaceMatrix);
    //return GameState::cam.getView();
    return lightProjection * lightSpaceMatrix;
}

int PointLight::LightsCount = 0;

glm::vec3 DirectionalLight::lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 DirectionalLight::lightPoint = glm::vec3(0.05f, 0.0f, 0.0f);
glm::vec3 DirectionalLight::direction = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec4 DirectionalLight::orthoBounds = glm::vec4(-10.0f, 10.0f, -10.0f, 10.0f);
