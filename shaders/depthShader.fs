#version 330 core
in vec4 FragPos;

#define MAX_SHADOWS 6

uniform vec3 lightPos[MAX_SHADOWS];
uniform float far_plane;
uniform int shadow;
uniform bool dynamic;

void main()
{
   float lightDistance = length(FragPos.xyz - lightPos[shadow]);

   lightDistance = lightDistance / far_plane;

   gl_FragDepth = lightDistance;
};