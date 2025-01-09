#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>

std::vector<float> cubeVert = {
    // Position         // Texture Coords // Normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  1.0f,  0.0f
};



std::vector<float> floorVert = {
    // Format for each vertex: position (3), texcoord (2), normal (3)
    // First triangle
    -100.0f, 0.0f, -100.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    100.0f,  0.0f, -100.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
    -100.0f, 0.0f,  100.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // top left
    
    // Second triangle
    100.0f,  0.0f, -100.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
    100.0f,  0.0f,  100.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // top right
    -100.0f, 0.0f,  100.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f   // top left
};