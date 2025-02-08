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
    int weaponID = 0;
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
    float reflectance = 0.0f;
    bool enabled = true; // Not very efficient way of cleaning things up but we won't be deleting too many objects dynamically
    bool visible = true; // Make the object completely visible or invisible (rendered / not rendered)
    bool selected = false;
    bool canCollide = true;
    bool dynamic = false;
    std::vector<float> temp_data; // Used for OpenGL to correctly parse my custom way of saving objects
    unsigned int VAO;
    unsigned int VBO;
};

std::vector<object> objects;

struct weapon // Later add ammo, and other customizations
{
    vertices vertices;
    std::vector<float> temp_data;
    unsigned int texture = 1;
    struct
    {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    } transform;

    struct
    {
        glm::vec3 pos = glm::vec3(1.0f, -1.5f, 0.25f);
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    } offset;

    glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
    std::string name = "Placeholder";
    unsigned int VAO;
    unsigned int VBO;

    // Stats

    // Shotgun
    bool shotgun = false;
    int pellets = 10;
    float spread = 7.5f; // Spread (radius of circle, not diameter)

    // Grenade launcher
    bool grenadeLauncher = false;
    float velocityStrength = 3.0f;

    // General
    bool explosive = false;
};

std::vector<weapon> weapons;

struct light // This truly is an ECS
{
    glm::vec3 pos;
    glm::vec3 color;
    int id;
    bool enabled = true;
    bool selected = false;
    float strength = 1.0f;

    // Shadows
    bool castShadow = true;
    int shadowID;
};

std::vector<light> lightArray;

#endif // STRUCTS_H