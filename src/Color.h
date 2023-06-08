#pragma once

struct Color
{
	glm::vec3 color;	
	operator glm::vec3()
	{
		return color;
	}
};

struct Material
{
    glm::vec3 ambient = {0,0,0};
    glm::vec3 diffuse = {0,0,0};
    glm::vec3 specular = {0,0,0};
    float shininess = 0;

    void setShaderMaterial(Shader sv)
    {
        sv.setInt("material.diffuse", 0);
        sv.setInt("material.specular", 1);

        //sv.setVec3("material.specular", specular); 
        sv.setFloat("material.shininess", shininess);	
    }
};
