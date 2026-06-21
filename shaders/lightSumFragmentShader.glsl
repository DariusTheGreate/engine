#version 410 core

//out vec4 FragColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

//NOTE(darius) for some weird reason this shit sometimes needs location specifyed, but sometimes brokes beacuse of it. I commented layouts for now, cause it only works this way.
//layout (location = 0) in vec2 TexCoords;
//layout (location = 1) in vec3 Normal;
//layout (location = 2) in vec3 FragPos;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in mat3 TBN;

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
uniform bool shadowCaster;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;   // map_Ka (ambient) map
uniform sampler2D texture_opacity1;  // map_d (alpha mask)
uniform sampler2D texture_disp1;     // reconstructed displacement/height (grayscale)
uniform sampler2D depthMap;

uniform int texture_diffuse_was_set;
uniform int texture_normal_was_set;
uniform int texture_specular_was_set;
uniform int texture_height_was_set;
uniform int texture_opacity_was_set;
uniform int texture_disp_was_set;

// per-mesh phong exponent from the material (Ns); 0 means "not provided"
uniform float meshShininess;

// parallax occlusion mapping (height sampled from the normal/_ddn map alpha)
uniform bool  parallaxEnabled;
uniform float parallaxHeightScale;

uniform float gammaBrightness;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    //projCoords = projCoords * 0.1;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    float bias = 0.025;
    if(closestDepth + bias < currentDepth)
        return 0.95;
    return 0.8;
}

// UV actually sampled by the material (may be parallax-offset). Set in main().
vec2 finalUV;

// Steep / occlusion parallax. Height is stored in the alpha of the _ddn normal map.
vec2 parallaxMapping(vec2 texCoords, vec3 viewDirTS)
{
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDirTS)));

    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 P = viewDirTS.xy * parallaxHeightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = 1.0 - texture(texture_disp1, currentTexCoords).r;

    for(int i = 0; i < 64; ++i)
    {
        if(currentLayerDepth >= currentDepthMapValue)
            break;
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(texture_disp1, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    // interpolate between the last two layers for a smooth result
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(texture_disp1, prevTexCoords).r) - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    return mix(currentTexCoords, prevTexCoords, weight);
}

void main()
{
    /*if(texture_normal_was_set == 666){
        vec4 color = texture(texture_normal1, TexCoords);

        if(color.a < 0.1)
            discard;

        FragColor.rgb = color.rgb;
        return;
    }
    else
    {
        FragColor.rgb = vec3(0.0f,0.0f,1.0f);
        return;
    }*/

    if(texture_diffuse_was_set != 666)
    {
        FragColor.rgb = vec3(1.0f,0.0f,0.0f);
            return;
    }

    // --- parallax occlusion mapping (height in normal/_ddn map alpha) ---
    finalUV = TexCoords;
    if(parallaxEnabled && parallaxHeightScale > 0.0 && texture_disp_was_set == 666)
    {
        // world-space view dir transformed into tangent space (TBN is orthonormal)
        vec3 viewDirTS = normalize(transpose(TBN) * (viewPos - FragPos));
        finalUV = parallaxMapping(TexCoords, viewDirTS);
        // NOTE: no edge discard here — Sponza textures are tiled (UVs outside [0,1] with
        // GL_REPEAT), so discarding out-of-[0,1] fragments would erase the walls.
    }

    // --- alpha-mask cutout (map_d / OPACITY), e.g. Sponza foliage & chains ---
    if(texture_opacity_was_set == 666 && texture(texture_opacity1, finalUV).r < 0.5)
        discard;

    vec4 texColor = texture(texture_diffuse1, finalUV);
    if(texColor.a < 0.1)
        discard;

    vec3 norm = vec3(0.0f,0.0f,1.0f);

    if(texture_normal_was_set == 666){
        // tangent-space normal map: decode [0,1] -> [-1,1], then to world space via TBN
        norm = texture(texture_normal1, finalUV).rgb;
        norm = normalize(TBN * (norm * 2.0 - 1.0));
    }
    else{
        // geometric normal is already a unit vector in [-1,1]; use as-is
        norm = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0f,0.0f,0.0f); //calcDirectionalLight(dirLight, norm, viewDir);

    for(int i = 0; i < lightsCount; i++)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);    

    float shadow = ShadowCalculation(FragPosLightSpace); 

    //if(!shadowCaster)
    //    result = (1.0 - shadow) * result;
        
    if(lightsCount == 0){
        FragColor = texColor;
    }
    else{
        //FragColor = gammaBrightness * texColor * vec4(result, 1.0f);
        FragColor = gammaBrightness *  vec4(result, 1.0f);
    }

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}   

// Phong exponent: per-mesh Ns first, then object-material override, else a sane default.
float effectiveShininess()
{
    if(meshShininess > 0.0)
        return meshShininess;
    if(material.shininess > 0.0)
        return material.shininess;
    return 32.0;
}

// Ambient color source: dedicated ambient map (map_Ka) if present, else the diffuse texel.
vec3 ambientTexel()
{
    if(texture_height_was_set == 666)
        return vec3(texture(texture_height1, finalUV));
    return vec3(texture(texture_diffuse1, finalUV));
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), effectiveShininess());

    vec3 ambient = light.ambient * ambientTexel();
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, finalUV));

    vec3 specular = vec3(0.0f,0.0f,0.0f);
    if(texture_specular_was_set == 666)
        specular = light.specular * spec * vec3(texture(texture_specular1, finalUV));

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), effectiveShininess());

    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

    float distance = length(light.position - fragPos);
    //distance /= 10;
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * ambientTexel();
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, finalUV));

    vec3 specular = vec3(0.0f,0.0f,0.0f);
    if(texture_specular_was_set == 666)
        specular = light.specular * spec * vec3(texture(texture_specular1, finalUV));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float intensity = 0.4 * spec;

    //if (intensity > 0.9) 
    //    intensity = 1.1;
    //else if (intensity > 0.5) 
    //    intensity = 0.7;
    //else 
    //    intensity = 0.5;


    return (ambient + diffuse + specular);// * intensity;
}