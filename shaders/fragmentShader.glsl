#version 410 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;

uniform vec3 viewPos;
uniform int lightsCount;
uniform float gammaFactor;

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
	//FragColor = vec4(1.0f,0.0f,1.0f,1.0f);//texture(texture_diffuse1, TexCoords);
    //return;
    
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    //if(texColor.a < 0.1)
        //discard;
    //FragColor.rgb = texColor.rbg;
    FragColor.rgb = pow((texture(texture_diffuse1, TexCoords)).rgb, vec3(1.0/gammaFactor));
}
