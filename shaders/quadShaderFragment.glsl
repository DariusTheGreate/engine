#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

vec2 getPixelizedCoords();

void main()
{ 
	//vec2 uv = TexCoords;
	vec2 uv = getPixelizedCoords();

    FragColor = texture(screenTexture, uv);
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}

vec2 getPixelizedCoords()
{
	int pixelSize = 6;

	vec2 texSize  = textureSize(screenTexture, 0).xy;
	vec2 texCoord = gl_FragCoord.xy / texSize;

	float x = int(gl_FragCoord.x) % pixelSize;
	float y = int(gl_FragCoord.y) % pixelSize;

    x = floor(pixelSize / 2.0) - x;
    y = floor(pixelSize / 2.0) - y;

    x = gl_FragCoord.x + x;
    y = gl_FragCoord.y + y;	

	vec2 uv = vec2(x, y) / texSize;

	return uv;
}
