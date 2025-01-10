#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D currentTexture;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float shininess = 32.0f;
   float intensity = 1.0f;
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
   vec3 specular = intensity * spec * lightColor;
   vec3 ambient = ambientStrength * lightColor;
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;
   float distance = length(lightPos - FragPos);
   float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));
   
   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;
   vec3 result = (ambient + diffuse + specular);
   FragColor = texture(currentTexture, TexCoord) * vec4(result, 1.0);
};