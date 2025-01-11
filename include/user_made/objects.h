#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>

// Linked list :money:
int currentIDNumber = 0;
int currentLightID = 0;


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
    glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
    unsigned int shader;
    bool light;
    unsigned int texture = 1;
    bool enabled = true; // Not very efficient way of cleaning things up but we won't be deleting too many objects dynamically
};


struct light // This truly is an ECS
{
    glm::vec3 pos;
    glm::vec3 color;
    int id;
    bool enabled = true;
};


std::vector<object> objects;
std::vector<light> lightArray;

void add_object(int id, std::string name, std::vector<float> vertices, bool isLight, unsigned int shader)
{
    object newObject;
    newObject.id = currentIDNumber;
    newObject.name = name + std::to_string(currentIDNumber);
    newObject.vertices = vertices;
    newObject.light = isLight;
    newObject.shader = shader;
    newObject.transform.pos = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    

    struct light newLight;
    newLight.pos = newObject.transform.pos;
    newLight.id = currentIDNumber;
    newLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    
    if (isLight == true)
    {
        lightArray.push_back(newLight);
        currentLightID += 1;
    }
    
    objects.push_back(newObject);
    currentIDNumber += 1;
}

void remove_object(int id)
{
    // Who needs memory anyway?
    objects[id].enabled = false;
    for (int i = 0; i < lightArray.size(); i++)
    {
        if (lightArray[i].id == id)
        {
            lightArray[i].enabled = false;
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

void SaveToFile(const std::string& filename)
{
    std::ofstream outfile(filename);
    
    // Write number of objects
    outfile << objects.size() << "\n";
    
    
    outfile.close();
}

void LoadFromFile(const std::string& filename)
{
    std::ifstream infile(filename);
    if (!infile.is_open()) {return;}

    std::string text;

    int objectNum;
    
    while (std::getline(infile, text))
    {
        printf("%s\n",text.c_str());
    }

    infile.close();
}