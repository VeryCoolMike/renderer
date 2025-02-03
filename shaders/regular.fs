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
} fs_in;

uniform samplerCube shadowMap;

#define MAX_LIGHTS 500

uniform sampler2D currentTexture;
uniform samplerCube skybox;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform bool selected;
uniform float reflectancy;
uniform vec3 lightPos;
uniform float far_plane;

uniform int lightAmount;
uniform PointLight pointLights[MAX_LIGHTS];

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

    float shadow = texture(shadowMap, fs_in.FragPos - lightPos).r < 0.3 ? 1.0 : 0.0; // Magic wizardry, How does this work, I don't know (taken from learnopengl, this shouldnt work but does)

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