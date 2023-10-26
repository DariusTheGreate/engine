#version 410 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16)/32.0f;
    
    if(h > 0.1)
	    FragColor = vec4(h, h, 1, 1.0);
   	if(h > 6.5) 
	    FragColor = vec4(h, 0, h, 1.0);
}
