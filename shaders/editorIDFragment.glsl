#version 420 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec4 PickingColor;

//uniform sampler2D UiMask;

void main(){
    FragColor = PickingColor;
    //FragColor = vec4(1.0f,1.0f,0.0f,0.0f);//PickingColor;
}

