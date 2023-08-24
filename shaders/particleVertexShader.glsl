#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;
layout (location = 4) in float aAlpha;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out float alpha;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    //gl_Position = projection * view * model * vec4(aPos.x + gl_InstanceID, aPos.y + gl_InstanceID, aPos.z + gl_InstanceID, 1.0);
    gl_Position = projection * view * model * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z + aOffset.z, 1.0);
    alpha = aAlpha;
}

