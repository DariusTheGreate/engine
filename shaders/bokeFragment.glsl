#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
    //vec4 texColor = texture(image, TexCoords);
    FragColor = vec4(1.0,0.0,1.0,1.0);
    return;

    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    return;
    
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0];

    float farPlane = 1;

    //if(gl_FragCoord.z > 0 && gl_FragCoord.z < farPlane){
    if(gl_FragCoord.z > farPlane){
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else{
        result = vec3(1.0f, 1.0f, 1.0f);
    }

    FragColor = vec4(result, 1.0);
}
