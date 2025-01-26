#ifndef STRUCTS_H
#define STRUCTS_H

#include <string.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>

struct player // Add other things later such as health and more
{
    int weaponID = 2;
};

struct gui
{
    int id;
    bool visible;
};

struct vertices
{
    std::vector<glm::vec3> position;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normal;
    std::string id;
};

struct object
{
    int id;
    std::string name;
    vertices vertices;
    struct
    {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    } transform;
    glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int shader;
    bool light;
    unsigned int texture = 1;
    bool enabled = true; // Not very efficient way of cleaning things up but we won't be deleting too many objects dynamically
    bool selected = false;
    bool canCollide = true;
    std::vector<float> temp_data;
    unsigned int VAO;
    unsigned int VBO;
};

#endif // STRUCTS_H