#version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    bool enabled;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

#define MAX_LIGHTS 10

uniform sampler2D currentTexture;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform bool fullBright;
uniform bool selected;

uniform int lightAmount;
uniform PointLight pointLights[MAX_LIGHTS];

vec3 calcPointLight(PointLight light)
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float shininess = 32.0;
    float intensity = 1.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = intensity * spec * light.color;
    
    vec3 ambient = ambientStrength * light.color;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular) * objectColor;
}

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (i < lightAmount)
        {
            if (pointLights[i].enabled == true)
            {
               result += calcPointLight(pointLights[i]);
            }
        }
    }
    
    
    if (fullBright)
    {
        FragColor = texture(currentTexture, TexCoord) * vec4(result, 1.0) + vec4(1.0);
    }
    else
    {
        FragColor = texture(currentTexture, TexCoord) * vec4(result, 1.0);
    }

    if (selected == true)
    {
        FragColor = texture(currentTexture, TexCoord);
    }
}