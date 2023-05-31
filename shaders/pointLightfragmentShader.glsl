#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;

uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_normal4;

uniform sampler2D texture_height1;
uniform sampler2D texture_height2;
uniform sampler2D texture_height3;
uniform sampler2D texture_height4;

void main()
{

    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 norm         = normalize(Normal);
    vec3 lightDir     = normalize(light.position - FragPos);
    float diff        = max(dot(norm, lightDir), 0.0);
    vec3 diffuse      = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)); 

    vec3 ambient      = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 viewDir      = normalize(viewPos - FragPos);
    vec3 reflectDir   = reflect(-lightDir, norm);  
    float spec        = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular     = light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient          *= attenuation; 
    diffuse          *= attenuation;
    specular         *= attenuation;  
        
    vec3 result       = ambient + diffuse + specular;
    FragColor         = texture(texture_diffuse1, TexCoords) * vec4(result, 1.0f);
}   
