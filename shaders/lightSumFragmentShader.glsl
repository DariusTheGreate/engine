#version 410 core

out vec4 FragColor;

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 FragPos;

struct Material {
    float shininess;
}; 

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform DirectionalLight dirLight;

#define NR_POINT_LIGHTS 21 
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int lightsCount;
uniform float gammaFactor;

uniform Material material;//NOTE(darius) diable foe now
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;

uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_normal4;

uniform sampler2D texture_height1;
uniform sampler2D texture_height2;
uniform sampler2D texture_height3;
uniform sampler2D texture_height4;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    //float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    //FragColor = vec4(vec3(depth), 1.0);
    //return;

    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    //return;

    //FragColor = vec4(1.0f,0.0f,1.0f,1.0f);//texture(texture_diffuse1, TexCoords);
    //return;

    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1)
        discard;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = calcDirectionalLight(dirLight, norm, viewDir);

    for(int i = 0; i < lightsCount; i++)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);    
        
   if(lightsCount == 0)
        FragColor = texColor;
    else
        FragColor = texColor * vec4(result, 1.0f);

    //gamma corecion
    //FragColor.rgb = pow((texture(texture_diffuse1, TexCoords) * vec4(result, 1.0f)).rgb, vec3(1.0/gammaFactor));
    //FragColor.rgb = 0.1 * (texture(texture_diffuse1, TexCoords)).rgb;
    //FragColor.a = 1.0;
}   

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
 
    //float intensity = 0.4 * spec;

 	//if (intensity > 0.9) 
 	//	intensity = 1.1;
 	//else if (intensity > 0.5) 
 	//	intensity = 0.7;
 	//else 
 	//	intensity = 0.5;


    return (ambient + diffuse + specular);// * intensity;
}
