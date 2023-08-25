#version 410 core

out vec4 FragColor;

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in float alpha;

struct Material {
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform DirectionalLight dirLight;

#define NR_POINT_LIGHTS 21 
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int lightsCount;
uniform float gammaFactor;

uniform Material material;//NOTE(darius) diable foe now
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

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    //float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    //FragColor = vec4(vec3(depth), 1.0);
    //return;

    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0)
        discard;

    FragColor = texColor;
    FragColor.a = texColor.a - alpha;
}   

