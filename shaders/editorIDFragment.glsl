#version 420 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec4 PickingColor;

void main(){
    FragColor = PickingColor;
}

