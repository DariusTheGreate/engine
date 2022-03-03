#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 0) in vec3 aPos;


void main()
{
    gl_Position = vec4(aPos, 1.0);
}
