#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT
{
   vec3 FragPos;
   vec3 Normal;
   vec2 TexCoord;
   vec4 FragPosLightSpace;
} gs_in[];

out VS_OUT // I need all this for some reason
{
   vec3 FragPos;
   vec3 Normal;
   vec2 TexCoord;
   vec4 FragPosLightSpace;
} gs_out;

void main() {    
    for (int i = 0; i < 3; i++) { // Have to loop through each vertice for some reason
        gs_out.FragPos = gs_in[i].FragPos;
        gs_out.Normal = gs_in[i].Normal;
        gs_out.TexCoord = gs_in[i].TexCoord;
        gs_out.FragPosLightSpace = gs_in[i].FragPosLightSpace;

        gl_Position = gl_in[i].gl_Position; // Correctly passing vertex positions
        EmitVertex();
    }

    EndPrimitive();
}
