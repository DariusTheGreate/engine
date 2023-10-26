#version 410 core

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    //gl_FragDepth = gl_FragCoord.z; //it basicly happens anyway
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    //FragColor = vec4(vec3(depth), 1.0);
    FragColor = vec4(vec3(depth*100), 1.0);

    //FragColor = vec4(1.0f,0.0f,1.0f,1.0f);//texture(texture_diffuse1, TexCoords);
}
