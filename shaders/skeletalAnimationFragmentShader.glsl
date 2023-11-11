#version 420 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;

layout(binding=0) uniform sampler2D texture_diffuse1;
layout(binding=1) uniform sampler2D texture_normal1;
layout(binding=2) uniform sampler2D texture_specular1;
layout(binding=3) uniform sampler2D texture_height1;
layout(binding=4) uniform sampler2D depthMap;


uniform int texture_diffuse_was_set;
uniform int texture_normal_was_set;
uniform int texture_specular_was_set;
uniform int texture_height_was_set;


void main()
{    
    
    if(texture_diffuse_was_set == 666){
        vec3 color = texture(texture_diffuse1, TexCoords).rgb;
        FragColor.rgb = color;
        return;
    }
    else
    {
        FragColor.rgb = vec3(0.0f,0.0f,1.0f);
        return;
    }
}
