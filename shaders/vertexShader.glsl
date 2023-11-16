#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace; 
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));

	//TODO: set default tangets here
	if(aTangent == vec3(0.0f, 0.0f, 0.0f)){
		//T = cross(normalize(N), vec3(1.0,0.0,0.0));
		//T = vec3(1.0, 0.0, 0.0);
		T = normalize(vec3(model * vec4(1.0, 0.0, 0.0, 0.0)));
	}

	if(aBitangent == vec3(0.0f, 0.0f, 0.0f)){
		//B = cross(normalize(N), vec3(0.0,1.0,0.0));
		//B = vec3(0.0, 1.0, 0.0);
		B = normalize(vec3(model * vec4(0.0, 1.0, 0.0, 0.0)));
	}

	if(aNormal == vec3(0.0, 0.0, 0.0)){
		//N = vec3(0.0, 0.0, 1.0);
		N = normalize(vec3(model * vec4(0.0, 0.0, 1.0, 0.0)));
	}


	TBN = mat3(T, B, N);

    TexCoords = aTexCoords;    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = transpose(inverse(mat3(model))) * aNormal;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

