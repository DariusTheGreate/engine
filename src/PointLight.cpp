#include <PointLight.h>
#include <Shader.h>

PointLight::PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in, float radius_in) : position(position_in), color(color_in), intensity(intensity_in), radius(radius_in) {
    //lightNumber = PointLight::LightsCount++;
}

void PointLight::addLight()
{
    lightNumber = PointLight::LightsCount++;
}

void PointLight::setShaderLight(Shader sv)
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

DirectionalLight::DirectionalLight(glm::vec3 direction_in, glm::vec3 color_in, float intensity_in, float radius_in) : direction(direction_in), color(color_in), intensity(intensity_in), radius(radius_in) {
    diffuse = color * glm::vec3(0.5f);
    ambient = diffuse * glm::vec3(0.2f);
}

void DirectionalLight::setShaderLight(Shader sv)
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

void SpotLight::setShaderLight(Shader sv)
{
    sv.setVec3("light.position", position);
    sv.setVec3("light.ambient", ambient);
    sv.setVec3("light.diffuse", diffuse);
    sv.setVec3("light.specular", specular);

    sv.setFloat("light.constant", constant);
    sv.setFloat("light.linear", linear);
    sv.setFloat("light.quadratic", quadratic);
}

int PointLight::LightsCount = 0;

