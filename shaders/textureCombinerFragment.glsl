#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D bloomBlur;

//uniform float exposure;

void main()
{             

    //FragColor = vec4(1.0f,0.0f,1.0f,1.0f);//texture(texture_diffuse1, TexCoords);
    //return;

    float exposure = 1.0f;

    //NOTE(darius) works
    vec3 color = texture(bloomBlur, TexCoords).rgb;
    FragColor.rgb = color; 
    return;

    const float gamma = 1.0;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    //result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}  

