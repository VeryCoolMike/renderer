#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>


// Linked list :money:
int currentIDNumber = 0;
int currentLightID = 0;

extern std::vector<float> cubeVert;

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
    bool selected = false;
};


struct light // This truly is an ECS
{
    glm::vec3 pos;
    glm::vec3 color;
    int id;
    bool enabled = true;
    bool selected = false;
};


std::vector<object> objects;
std::vector<light> lightArray;

object add_object(int id, std::string name, std::vector<float> vertices, bool isLight)
{
    object newObject;
    newObject.id = currentIDNumber;
    newObject.name = name;
    newObject.vertices = vertices;
    newObject.light = isLight;
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

    return newObject;
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

// This is going to be really difficult, isn't it? EDIT: Wasn't that difficult
void SaveToFile(const std::string& filename)
{
    std::ofstream outfile(filename);
    
    
    for(const auto& obj : objects) {

        outfile << obj.id << "\n" << obj.name << "\n" << obj.light << "\n"; // ID, Name, And IsLight
        outfile << obj.objectColor[0] << "\n" << obj.objectColor[1] << "\n" << obj.objectColor[2] << "\n"; // Lights colors!!!!
        outfile << obj.texture << "\n"; // Textures
        outfile << obj.transform.pos.x << "\n" << obj.transform.pos.y << "\n" << obj.transform.pos.z << "\n"; // Position things! >:3
        outfile << obj.transform.rot.x << "\n" << obj.transform.rot.y << "\n" << obj.transform.rot.z << "\n"; // rotation, basically useless, until it isn't
        outfile << obj.transform.scale.x << "\n" << obj.transform.scale.y << "\n" << obj.transform.scale.z << "\n"; // scales :(
        // 1. ID
        // 2. Name
        // 3. IsLight
        // 4. LightColorX
        // 5. LightColorY
        // 6. LightColorZ
        // 7. Texture
        // 8. PosX
        // 9. PosY
        // 10. PosZ
        // 11. RotX
        // 12. RotY
        // 13. RotZ
        // 14. ScaleX
        // 15. ScaleY
        // 16. ScaleZ
    }
    
    outfile.close();
}

// 0 is BAD Run this it's broken
int LoadFromFile(const std::string& filename) // add_object(int id, std::string name, std::vector<float> vertices, bool isLight, unsigned int shader)
{
    std::ifstream infile(filename);
    if (!infile.is_open()) {return 0;} // 0 means error, -1 would crash the program or something so 0 is a safe way of letting the computer know that something went wrong

    std::string text;

    int currentIDLine;
    int lineCount = 0;

    const int objectSize = 16;
    const int objectOffset = 0;

    int lightCounter = 0;

    int tempID;
    std::string tempName;
    bool tempIsLight;
    glm::vec3 tempTransformPos;
    glm::vec3 tempTransformScale;
    glm::vec3 tempTransformRot;
    glm::vec3 tempLightColor;
    int tempTexture;

    objects.clear();
    currentIDNumber = 0;
    currentLightID = 0;
    lightArray.clear();


    while (std::getline(infile, text))
    {
        lineCount += 1;
        if ((lineCount + objectOffset) % objectSize == 1) // Line 1 - ID
        {
            tempID = std::stoi(text);
        }
        else if ((lineCount + objectOffset) % objectSize == 2) // Line 2 - Name
        {
            tempName = text;
        }
        else if ((lineCount + objectOffset) % objectSize == 3) // Line 3 - IsLight
        {
            std::string buffer = text.c_str();
            if (buffer == "1") // Does not work EDIT: fixed :)
            {
                tempIsLight = true;
                lightCounter += 1;
            }
            else
            {
                tempIsLight = false;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 4) // Line 4 - Light 0
        {
            try {
                tempLightColor[0] = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting Light 0 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 5) // Line 5 - Light 1
        {
            try {
                tempLightColor[1] = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting Light 1 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 6) // Line 6 - Light 2
        {
            try {
                tempLightColor[2] = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting Light 2 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 7) // Line 7 - Texture
        {
            try {
                tempTexture = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting Texture value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 8) // Line 8 - PosX
        {
            try {
                tempTransformPos.x = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting PosX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 9) // Line 9 - PosY
        {
            try {
                tempTransformPos.y = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting PosY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 10) // Line 10 - PosZ
        {
            try {
                tempTransformPos.z = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting PosZ value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 11) // Line 11 - RotX
        {
            try {
                tempTransformRot.x = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting RotX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 12) // Line 12 - RotY
        {
            try {
                tempTransformRot.y = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting RotY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 13) // Line 13 - RotZ
        {
            try {
                tempTransformRot.z = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting RotZ value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 14) // Line 14 - ScaleX
        {
            try {
                tempTransformScale.x = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting ScaleX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 15) // Line 15 - ScaleY
        {
            try {
                tempTransformScale.y = std::stof(text);
            } catch (const std::exception& e) {
                std::cerr << "Error converting ScaleY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 0) // Line 16 - ScaleZ
        {
            try {
                tempTransformScale.z = std::stof(text.c_str());
                add_object(tempID, tempName, cubeVert, tempIsLight);
                objects.back().transform.pos = tempTransformPos;
                objects.back().transform.scale = tempTransformScale;
                objects.back().transform.rot = tempTransformRot;
                objects.back().texture = tempTexture;
                
                if (objects.back().light == true) // This can't be applied to everything or else lights break for some reason???
                {
                    objects.back().objectColor = tempLightColor;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error converting ScaleZ value to float: " << e.what() << std::endl;
            }
        }

    }

    std::cout << "File lines: " << lineCount << std::endl;
    std::cout << "Object size: " << objectSize << std::endl;
    std::cout << "Object offset: " << objectOffset << std::endl;
    return lightCounter;

    infile.close();
}


