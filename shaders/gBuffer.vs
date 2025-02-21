#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   vec4 worldPos = model * vec4(aPos, 1.0);
   FragPos = worldPos.xyz;
   TexCoord = aTexCoord;
   
   mat3 normalMatrix = transpose(inverse(mat3(model)));
   Normal = normalMatrix * aNormal;

   gl_Position = projection * view * worldPos;
};