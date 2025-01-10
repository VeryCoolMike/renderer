#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>

// Linked list :money:
int currentIDNumber = 0;


struct transform
{
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

struct object
{
    int id;
    std::string name;
    std::vector<float> vertices;
    struct {
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    } transform;
    unsigned int shader;
    bool light;
    unsigned int texture = 1;
};

std::vector<object> objects;

void add_object(int id, std::string name, std::vector<float> vertices, bool light, unsigned int shader)
{
    object newObject;
    newObject.id = currentIDNumber;
    newObject.name = name + std::to_string(currentIDNumber);
    newObject.vertices = vertices;
    newObject.light = light;
    newObject.shader = shader;
    newObject.transform.pos = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    objects.push_back(newObject);
    currentIDNumber += 1;
}

void remove_object(int id)
{
    // O(n) :skull:
    for (int i = 0; i < currentIDNumber; i++)
    {
        if (objects[i].id == id)
        {
            objects.erase(objects.begin() + i);
            currentIDNumber -= 1;
            return;
        }
    }
}

object& get_object_by_name(const std::string& name)
{
    // O(n) :skull:
    for (int i = 0; i < currentIDNumber; i++)
    {
        if (objects[i].name == name)
        {
            return objects[i]; // Returns a reference.
        }
    }

    throw std::runtime_error("Object not found");
}
