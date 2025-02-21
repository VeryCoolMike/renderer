#version 400 core
out vec4 FragColor;

struct PointLight
{
    vec3 position;
    vec3 color;
    bool enabled;
    bool castShadow;
    //int lightType;
    // 0 = Directional light
    // 1 = Point light
    // 2 = Spot light
    int shadowID;
    float strength;
};


#define MAX_LIGHTS 500
#define MAX_SHADOWS 6

in vec2 TexCoord;

uniform samplerCube shadowMap[MAX_SHADOWS];
uniform samplerCube dynamicShadowMap[MAX_SHADOWS];
uniform vec3 lightPos[MAX_SHADOWS];

uniform sampler2D currentTexture;
uniform samplerCube skybox;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform vec3 viewPos;
uniform bool selected;
uniform float reflectancy;
uniform float far_plane;
uniform bool shadowsEnabled;
uniform bool shadowDebug;
uniform int screenX;
uniform int screenY;

uniform int lightAmount;
uniform PointLight pointLights[MAX_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

float ShadowCalculation(vec3 fragPos, int id)
{

    vec3 fragToLight = fragPos - lightPos[id];
    float currentDepth = length(fragToLight);
    if (currentDepth >= far_plane)
    {
        return 2.0;
    }

    float bias = 0.15f;
    float viewDistance = length(viewPos - fragPos);

    float staticDepth = texture(shadowMap[id], fragToLight).r;
    staticDepth *= far_plane;

    float dynamicDepth = texture(dynamicShadowMap[id], fragToLight).r;
    dynamicDepth *= far_plane;

    float shadowStatic = (currentDepth - bias > staticDepth) ? 1.0 : 0.0;
    float shadowDynamic = (currentDepth - bias > dynamicDepth) ? 1.0 : 0.0;

    float shadow = max(shadowStatic, shadowDynamic);

    if (shadowDebug == true)
    {
        FragColor = vec4(vec3(shadowStatic), 1.0);
    }
    

    return shadow;
}

vec3 calcPointLight(PointLight light, vec3 FragPos, vec3 Normal, vec3 Albedo)
{

    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * light.color;

    float shininess = 32.0;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), shininess) * light.color;

    mediump float distance = length(light.position - FragPos);
    mediump float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = 0.0f;

    if (shadowsEnabled == true)
    {
        if (light.castShadow == true)
        {
            shadow = ShadowCalculation(FragPos, light.shadowID);
        }
    }
    
    return (1.0 - shadow) * (diffuse + specular) * objectColor;
}

void main()
{
    vec2 screenSize = vec2(screenX, screenY);
    vec3 FragPos = texture(gPosition, gl_FragCoord.xy / screenSize).rgb;
    vec3 Normal = texture(gNormal, gl_FragCoord.xy / screenSize).rgb;
    vec3 Albedo = texture(gAlbedo, gl_FragCoord.xy / screenSize).rgb;
    
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = Albedo * ambientStrength;
    
    for (int i = 0; i < lightAmount; i++)
    {
        if (pointLights[i].enabled == true)
        {
            result += calcPointLight(pointLights[i], FragPos, Normal, Albedo) * pointLights[i].strength;
        }
    }


    if (shadowDebug == false)
    {

        vec3 I = normalize(FragPos - viewPos) * reflectancy;
        vec3 R = reflect(I, normalize(Normal));

        FragColor = (vec4(result, 1.0)) * (vec4(texture(skybox, R).rgb, 1.0f));

    }

    if (selected == true)
    {
        FragColor = vec4(Albedo, 1.0);
    }
}