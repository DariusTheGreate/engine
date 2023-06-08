#pragma once

#include <glm/glm.hpp>
#include <string>

#include <Shader.h>

class PointLight{
public:
	PointLight()
    {
        //lightNumber = PointLight::LightsCount++;
    }

	PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f) : position(position_in), color(color_in), intensity(intensity_in), radius(radius_in) {
        //lightNumber = PointLight::LightsCount++;
	}

    void addLight()
    {
        lightNumber = PointLight::LightsCount++;
    }

	void setShaderLight(Shader sv)
    {
        diffuse = color  * glm::vec3(colorFactor); 
        ambient = diffuse* glm::vec3(diffuseFactor); 

    	sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].position", position);
    	sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].ambient", ambient);
        sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].diffuse", diffuse);
        sv.setVec3("pointLights[" + std::to_string(lightNumber) + "].specular", specular); 

        sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].constant",  constant);
		sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].linear",    linear);
		sv.setFloat("pointLights[" + std::to_string(lightNumber) + "].quadratic", quadratic);	
    }

	//TODO(darius) make it Tranform?
	glm::vec3 position = {0,0,0};
	glm::vec3 color = {1,1,1};

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular = {1,1,1};

    float constant = 1.0f;
    float linear = 0.09;
    float quadratic = 0.032;

	float intensity = 1.0f;	
	float radius = 1.0f;

    float colorFactor = 0.5f;
    float diffuseFactor = 0.5f;

    int lightNumber = 0;

    //todo(darius) sync 
    static int LightsCount;
};


class DirectionalLight{
public:
	DirectionalLight() = default;
	DirectionalLight(glm::vec3 direction_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f) : direction(direction_in), color(color_in), intensity(intensity_in), radius(radius_in) {
		diffuse = color  * glm::vec3(0.5f); 
        ambient = diffuse* glm::vec3(0.2f); 
	}

	void setShaderLight(Shader sv)
    {
    	sv.setVec3("dirLight.direction", direction);
    	sv.setVec3("dirLight.ambient", ambient);
        sv.setVec3("dirLight.diffuse", diffuse);
        sv.setVec3("dirLight.specular", specular); 
    }

	glm::vec3 direction = {-0.2f, -1.0f, -0.3f};
	glm::vec3 color = {1,1,1};

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular = {1,1,1};

	float intensity = 1.0f;
	float radius = 1.0f;
};

class SpotLight{
public:
	SpotLight() = default;
	SpotLight(glm::vec3 position_in, glm::vec3 direction_in, glm::vec3 color = glm::vec3(1,1,1), float radius_in = glm::cos(glm::radians(12.5f))) : position(position_in), direction(direction_in), radius(radius_in) {
		diffuse = color  * glm::vec3(0.5f); 
        ambient = diffuse* glm::vec3(0.2f); 
	}

	void setShaderLight(Shader sv)
    {
    	sv.setVec3("light.position", position);
    	sv.setVec3("light.ambient", ambient);
        sv.setVec3("light.diffuse", diffuse);
        sv.setVec3("light.specular", specular); 

        sv.setFloat("light.constant",  constant);
		sv.setFloat("light.linear",    linear);
		sv.setFloat("light.quadratic", quadratic);	
    }

	glm::vec3 position = {0,0,0};
	glm::vec3 direction = {0,-1,0};
	float radius = glm::cos(glm::radians(12.5f));

    float constant = 1.0f;
    float linear = 0.7;
    float quadratic = 1.8;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular = {1,1,1};
};
