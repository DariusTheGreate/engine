#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Shader;

class PointLight{
public:
    PointLight() = default;

    PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f);

    void addLight();

	void setShaderLight(Shader sv);

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

    void setShaderLight(Shader sv);

	glm::vec3 direction = {-0.2f, -1.0f, -0.3f};
	glm::vec3 color = {1,1,1};

    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {1,1,1};

	float intensity = 1.0f;
	float radius = 1.0f;

    static glm::vec3 lightPos;

    static glm::mat4 getLightMat() 
    {
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        return lightSpaceMatrix;
    }
};

class SpotLight{
public:
	SpotLight() = default;
    SpotLight(glm::vec3 position_in, glm::vec3 direction_in, glm::vec3 color = glm::vec3(1, 1, 1), float radius_in = glm::cos(glm::radians(12.5f)));

    void setShaderLight(Shader sv);

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

