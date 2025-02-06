#version 400 core
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

#define MAX_LIGHTS 500
#define MAX_SHADOWS 5

uniform samplerCube shadowMap[MAX_SHADOWS];
uniform vec3 lightPos[MAX_SHADOWS];

uniform sampler2D currentTexture;
uniform samplerCube skybox;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform bool selected;
uniform float reflectancy;
uniform float far_plane;

uniform int lightAmount;
uniform PointLight pointLights[MAX_LIGHTS];

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, int id)
{
    vec3 fragToLight = fragPos - lightPos[id];
    float closestDepth = texture(shadowMap[id], fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.15f;

    float viewDistance = length(viewPos - fragPos);
    float shadow = 0.0f;
    int samples = (viewDistance < far_plane * 0.5) ? 20 : 10;
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for (int i = 0; i < samples; i++)
    {
        float closestDepth = texture(shadowMap[id], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth)
        {
            shadow += 1.0f;
        }
    }
    shadow /= float(samples);
        
    return shadow;
}

vec3 calcPointLight(PointLight light)
{
    mediump float distance = length(light.position - fs_in.FragPos);
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
    
    mediump float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    mediump float shadow = 0.0f;

    shadow += ShadowCalculation(fs_in.FragPos, 0);
    shadow += ShadowCalculation(fs_in.FragPos, 1);
    shadow += ShadowCalculation(fs_in.FragPos, 2);

    
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


    if (reflectancy > 0.0)
    {
        vec3 I = normalize(fs_in.FragPos - viewPos) * reflectancy;
        vec3 R = reflect(I, normalize(fs_in.Normal));

        FragColor = (texture(currentTexture, fs_in.TexCoord) * vec4(result, 1.0)) * (vec4(texture(skybox, R).rgb, 1.0f));
    }
    else
    {
        FragColor = (texture(currentTexture, fs_in.TexCoord) * vec4(result, 1.0));
    }
    



    
    //float depth = length(fs_in.FragPos - lightPos);
    //FragColor = vec4(vec3(depth / (far_plane)), 1.0);

    if (selected == true)
    {
        FragColor = texture(currentTexture, fs_in.TexCoord);
    }
}