#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    //float depthValue = texture(depthMap, TexCoords).r;
    //only in r channel, cant use a
    float depthValueWeGet = texture(depthMap, TexCoords).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    //FragColor = vec4(vec3(depthValue), 1.0); // orthographic
    //FragColor = vec4(1.0f,0.0f,1.0f,1.0f);//texture(texture_diffuse1, TexCoords);
    FragColor = vec4(vec3(depthValueWeGet), 1.0f);
}
