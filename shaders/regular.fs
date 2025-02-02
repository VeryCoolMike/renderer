#version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    bool enabled;
    float strength;
};

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

#define MAX_LIGHTS 500

uniform sampler2D currentTexture;
uniform samplerCube skybox;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform bool selected;
uniform float reflectancy;

uniform int lightAmount;
uniform PointLight pointLights[MAX_LIGHTS];

float shadowCalculation(vec4 FragPosLightSpace)
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005f;
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if (projCoords.z > 1.0f)
    {
        shadow = 0.0f;
    }

    return shadow;
}

vec3 calcPointLight(PointLight light)
{
    float distance = length(light.position - fs_in.FragPos);
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float shininess = 32.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * light.color;
    
    vec3 ambient = ambientStrength * light.color;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;
    
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;


    float shadow = shadowCalculation(fs_in.FragPosLightSpace);

    
    
    return (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor; 
}

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < lightAmount; i++)
    {
        if (pointLights[i].enabled == true)
        {
            result += calcPointLight(pointLights[i]) * pointLights[i].strength;
        }
    }

    vec3 I = normalize(fs_in.FragPos - viewPos) * reflectancy;
    vec3 R = reflect(I, normalize(fs_in.Normal));

    FragColor = (texture(currentTexture, fs_in.TexCoord) * vec4(result, 1.0)) * (vec4(texture(skybox, R).rgb, 1.0f));

    if (selected == true)
    {
        FragColor = texture(currentTexture, fs_in.TexCoord);
    }
}