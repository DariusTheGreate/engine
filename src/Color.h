#pragma once

#include <iostream>

struct Color
{
	glm::vec3 color;	
	operator glm::vec3()
	{
		return color;
	}
};

//TODO(darius) NOTE(darius) disabled in shader code for now.
struct Material
{
    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {1,1,1};
    float shininess = 0;

    Material() = default;

    Material(float s) : shininess(s)
    {
    }

    void setShaderMaterial(Shader sv)
    {
        //TODO(darius) currently in shader there is samplers for diffuse and specular. 
        /*sv.setVec3("material.diffuse", diffuse.x, diffuse.y, diffuse.z);
        sv.setVec3("material.specular", specular.x, specular.y, specular.z);
        sv.setVec3("material.ambient", ambient.x, ambient.y, ambient.z);
        */

        sv.setFloat("material.shininess", shininess);	
    }
};

