#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>



// Glorified cube vertice storer, this file is useless but needed
struct vertices;
vertices cubeVert
{
    {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),

        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3(0.5f, -0.5f,  0.5f),
        glm::vec3(0.5f,  0.5f,  0.5f), 
        glm::vec3(0.5f,  0.5f,  0.5f),  
        glm::vec3(-0.5f,  0.5f,  0.5f), 
        glm::vec3(-0.5f, -0.5f,  0.5f), 

        glm::vec3(-0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f), 
        glm::vec3(-0.5f, -0.5f, -0.5f), 
        glm::vec3(-0.5f, -0.5f, -0.5f), 
        glm::vec3(-0.5f, -0.5f,  0.5f), 
        glm::vec3(-0.5f,  0.5f,  0.5f), 

        glm::vec3(0.5f,  0.5f,  0.5f), 
        glm::vec3(0.5f,  0.5f, -0.5f),  
        glm::vec3(0.5f, -0.5f, -0.5f),  
        glm::vec3(0.5f, -0.5f, -0.5f),  
        glm::vec3(0.5f, -0.5f,  0.5f),  
        glm::vec3(0.5f,  0.5f,  0.5f),  

        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),  
        glm::vec3(0.5f, -0.5f,  0.5f),  
        glm::vec3(0.5f, -0.5f,  0.5f),  
        glm::vec3(-0.5f, -0.5f,  0.5f), 
        glm::vec3(-0.5f, -0.5f, -0.5f), 

        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3(0.5f,  0.5f, -0.5f),  
        glm::vec3(0.5f,  0.5f,  0.5f),  
        glm::vec3(0.5f,  0.5f,  0.5f),  
        glm::vec3(-0.5f,  0.5f,  0.5f), 
        glm::vec3(-0.5f,  0.5f, -0.5f) 
    },

    {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f), 
        glm::vec2(1.0f, 1.0f), 
        glm::vec2(1.0f, 1.0f), 
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),

        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),

        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),

        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f)
    },

    {
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -1.0f),
        glm::vec3(0.0f,  0.0f, -1.0f),

        glm::vec3(-0.0f,  0.0f, 1.0f),
        glm::vec3(0.0f,  0.0f,  1.0f),
        glm::vec3(0.0f,  0.0f,  1.0f),
        glm::vec3(0.0f,  0.0f,  1.0f),
        glm::vec3(0.0f,  0.0f,  1.0f),
        glm::vec3(-0.0f,  0.0f, 1.0f),

        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),

        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),

        glm::vec3(0.0f, -1.0f,  0.0f),
        glm::vec3(0.0f, -1.0f,  0.0f),
        glm::vec3(0.0f, -1.0f,  0.0f),
        glm::vec3(0.0f, -1.0f,  0.0f),
        glm::vec3(0.0f, -1.0f,  0.0f),
        glm::vec3(0.0f, -1.0f,  0.0f),

        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f),
        glm::vec3(0.0f,  1.0f,  0.0f)
    }
};


