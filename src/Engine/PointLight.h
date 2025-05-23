#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <Engine/GameState.h>
#include <Core/Printer.h>

class Shader;

class PointLight{
public:
    PointLight() = default;

    PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f);

    void addLight();

	void setShaderLight(const Shader& sv);

	//TODO(darius) make it Tranform?
	glm::vec3 position = {0,0,0};
	glm::vec3 color = {1,1,1};

    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {1,1,1};

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

	float intensity = 1.0f;	
	float radius = 1.0f;

    float diffuseFactor = 0.5f;
    float ambientFactor = 0.5f;

    int lightNumber = 0;

    //todo(darius) sync
    //todo(darius) curently in scripting this sheet is not updates. Cause we make static library from this project, and then use it in scripting project. This way scripting projects has its own lightscount.
    static int LightsCount;
};

class DirectionalLight{
public:
	DirectionalLight() = default;
    DirectionalLight(glm::vec3 direction_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f);

    void setShaderLight(const Shader& sv);


	glm::vec3 color = {1,1,1};

    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {1,1,1};

	float intensity = 1.0f;
	float radius = 1.0f;

    static glm::vec3 lightPos;
    static glm::vec3 direction;
    static glm::vec3 lightPoint;

    static glm::vec4 orthoBounds;

    static glm::mat4 getLightMat(); 
};

class SpotLight{
public:
	SpotLight() = default;
    SpotLight(glm::vec3 position_in, glm::vec3 direction_in, glm::vec3 color = glm::vec3(1, 1, 1), float radius_in = glm::cos(glm::radians(12.5f)));

    void setShaderLight(const Shader& sv);

	glm::vec3 position = {0,0,0};
	glm::vec3 direction = {0,-1,0};
	float radius = glm::cos(glm::radians(12.5f));

    float constant = 1.0f;
    float linear = 0.7f;
    float quadratic = 1.8f;

    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {1,1,1};
};

