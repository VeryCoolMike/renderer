#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 36) out;

#define MAX_SHADOWS 6

uniform mat4 shadowMatrices[5][6];

uniform int shadow;

out vec4 FragPos;

void main() {  

    for (int face = 0; face < 6; face++)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; i++) { // Have to loop through each vertice for some reason
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[shadow][face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    } 
}