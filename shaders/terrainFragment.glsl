#version 410 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    if(h > 0.2)
	    FragColor = vec4(h, h, 0, 1.0);
    else
	    FragColor = vec4(h, h, 1, 1.0);
}
