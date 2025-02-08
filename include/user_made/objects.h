#ifndef OBJECTS_H
#define OBJECTS_H

#include "structs.h"
#include "render.h"

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

#include <vector>

float quadVertices[] = 
{
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

unsigned int quadVAO, quadVBO;

int currentIDNumber = 0;
int currentLightID = 0;
int currentObjID = 0;

extern std::vector<gui> guisVisible;


std::vector<float> convertGLMToOpenGLFLoat(vertices vertices)
{
    std::vector<float> temp_data;

    for (int v = 0; v < vertices.position.size(); v++)
    {
        // Positions
        temp_data.push_back(vertices.position[v].x);
        temp_data.push_back(vertices.position[v].y);
        temp_data.push_back(vertices.position[v].z);

        // Texture Coordinates
        temp_data.push_back(vertices.texCoords[v][0]);
        temp_data.push_back(vertices.texCoords[v][1]);

        // Normals
        temp_data.push_back(vertices.normal[v][0]);
        temp_data.push_back(vertices.normal[v][1]);
        temp_data.push_back(vertices.normal[v][2]);
    }

    return temp_data;
}

object add_object(int id, std::string name, vertices vertices, bool isLight)
{
    object newObject;
    newObject.id = currentIDNumber;
    newObject.name = name;
    newObject.vertices = vertices;
    newObject.light = isLight;
    newObject.transform.pos = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.rot = glm::vec3(0.0f, 0.0f, 0.0f);
    newObject.transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    newObject.VAO = VAO;
    newObject.VBO = VBO;

    std::vector<float> temp_data = convertGLMToOpenGLFLoat(vertices);

    newObject.temp_data = temp_data;

    glBufferData(GL_ARRAY_BUFFER, newObject.temp_data.size() * sizeof(float), newObject.temp_data.data(), GL_STATIC_DRAW);
    int vertexsize = 8; // I hate doing this manually

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    struct light newLight;
    newLight.pos = newObject.transform.pos;
    newLight.id = currentIDNumber;
    newLight.color = glm::vec3(1.0f, 1.0f, 1.0f);

    if (isLight == true)
    {

        lightArray.push_back(newLight);
        currentLightID += 1;
    }

    for (int i = 0; i < guisVisible.size(); i++)
    {
        guisVisible[i].visible = false;
    }

    gui newGui;
    newGui.id = currentIDNumber;
    newGui.visible = false;
    guisVisible.push_back(newGui);
    std::cout << guisVisible.back().id << std::endl;

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

object &get_object_by_name(const std::string &name)
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
void SaveToFile(const std::string &filename)
{
    std::ofstream outfile(filename);

    for (const auto &obj : objects)
    {

        outfile << obj.id << "\n"
                << obj.name << "\n"
                << obj.light << "\n"; // ID, Name, And IsLight
        outfile << obj.objectColor[0] << "\n"
                << obj.objectColor[1] << "\n"
                << obj.objectColor[2] << "\n"; // Lights colors!!!!
        outfile << obj.texture << "\n";        // Textures
        outfile << obj.transform.pos.x << "\n"
                << obj.transform.pos.y << "\n"
                << obj.transform.pos.z << "\n"; // Position things! >:3
        outfile << obj.transform.rot.x << "\n"
                << obj.transform.rot.y << "\n"
                << obj.transform.rot.z << "\n"; // rotation, basically useless, until it isn't
        outfile << obj.transform.scale.x << "\n"
                << obj.transform.scale.y << "\n"
                << obj.transform.scale.z << "\n"; // scales :(
        outfile << obj.vertices.id << "\n";       // Object ID
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
        // 17. Object ID
    }

    outfile.close();
}

vertices loadObj(const std::string &filename) // piss up
{
    std::ifstream infile(filename);
    vertices emptyVertices;
    if (!infile.is_open())
    {
        std::cout << "ERROR! Object trying to load: " << filename << " does not exist!" << std::endl;
        return emptyVertices;
    }

    std::string text;

    vertices currentVertices;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texture_coords;
    std::vector<glm::vec3> normals;

    int count = 0;
    bool counted = false;

    while (std::getline(infile, text))
    {
        std::istringstream iss(text);
        while (iss)
        {
            std::string prefix;
            iss >> prefix;
            if (prefix == "v") // Vertices
            {
                float x, y, z;
                if (iss >> x >> y >> z)
                {
                    positions.push_back(glm::vec3(x, y, z));
                }
                count += 1;
            }
            else if (prefix == "vt") // Vertices tetures
            {
                float x, y;
                if (iss >> x >> y)
                {
                    texture_coords.push_back(glm::vec2(x, y));
                }
            }
            else if (prefix == "vn") // Vertices normals
            {
                float x, y, z;
                if (iss >> x >> y >> z)
                {
                    normals.push_back(glm::vec3(x, y, z));
                }
            }
            else if (prefix == "f") // This is so peak
            {
                // Create 16 vertices (9 for triangle, extra 3 for quads)
                int v1, vt1, vn1;
                int v2, vt2, vn2;
                int v3, vt3, vn3;
                int v4, vt4, vn4;

                char slash; // slash character (istringstream only seperates on white spaces)

                // Create the 9 vertices
                iss >> v1 >> slash >> vt1 >> slash >> vn1;
                currentVertices.position.push_back(positions[v1 - 1]);
                currentVertices.texCoords.push_back(texture_coords[vt1 - 1]);
                currentVertices.normal.push_back(normals[vn1 - 1]);

                iss >> v2 >> slash >> vt2 >> slash >> vn2;
                currentVertices.position.push_back(positions[v2 - 1]);
                currentVertices.texCoords.push_back(texture_coords[vt2 - 1]);
                currentVertices.normal.push_back(normals[vn2 - 1]);

                iss >> v3 >> slash >> vt3 >> slash >> vn3;
                currentVertices.position.push_back(positions[v3 - 1]);
                currentVertices.texCoords.push_back(texture_coords[vt3 - 1]);
                currentVertices.normal.push_back(normals[vn3 - 1]);

                if (iss >> v4 >> slash >> vt4 >> slash >> vn4) // Incase of quad
                {
                    // Create the extra 3 vertices
                    iss >> v4 >> slash >> vt4 >> slash >> vn4;

                    iss >> v1 >> slash >> vt1 >> slash >> vn1;
                    currentVertices.position.push_back(positions[v1 - 1]);
                    currentVertices.texCoords.push_back(texture_coords[vt1 - 1]);
                    currentVertices.normal.push_back(normals[vn1 - 1]);

                    iss >> v4 >> slash >> vt4 >> slash >> vn4;
                    currentVertices.position.push_back(positions[v4 - 1]);
                    currentVertices.texCoords.push_back(texture_coords[vt4 - 1]);
                    currentVertices.normal.push_back(normals[vn4 - 1]);

                    iss >> v3 >> slash >> vt3 >> slash >> vn3;
                    currentVertices.position.push_back(positions[v3 - 1]);
                    currentVertices.texCoords.push_back(texture_coords[vt3 - 1]);
                    currentVertices.normal.push_back(normals[vn3 - 1]);
                }
            }
        }
    }

    infile.close();

    currentVertices.id = filename;
    std::cout << currentVertices.id << std::endl;

    return currentVertices;
}

// 0 is BAD Run this it's broken
int LoadFromFile(const std::string &filename) // Load a map from a text file
{
    std::ifstream infile(filename);
    if (!infile.is_open())
    {
        return 0;
    } // 0 means error, -1 would crash the program or something so 0 is a safe way of letting the computer know that something went wrong
      // Coming back, wtf is this meant to mean???
    std::string text;

    int currentIDLine;
    int lineCount = 0;

    const int objectSize = 17;
    const int objectOffset = 0;

    int lightCounter = 0;

    // Making temporary variables for when creating an object
    int tempID;
    std::string tempName;
    bool tempIsLight;
    glm::vec3 tempTransformPos;
    glm::vec3 tempTransformScale;
    glm::vec3 tempTransformRot;
    glm::vec3 tempLightColor;
    int tempTexture;

    // Clearing the current objects
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
            try
            {
                tempLightColor[0] = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting Light 0 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 5) // Line 5 - Light 1
        {
            try
            {
                tempLightColor[1] = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting Light 1 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 6) // Line 6 - Light 2
        {
            try
            {
                tempLightColor[2] = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting Light 2 value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 7) // Line 7 - Texture
        {
            try
            {
                tempTexture = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting Texture value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 8) // Line 8 - PosX
        {
            try
            {
                tempTransformPos.x = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting PosX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 9) // Line 9 - PosY
        {
            try
            {
                tempTransformPos.y = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting PosY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 10) // Line 10 - PosZ
        {
            try
            {
                tempTransformPos.z = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting PosZ value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 11) // Line 11 - RotX
        {
            try
            {
                tempTransformRot.x = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting RotX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 12) // Line 12 - RotY
        {
            try
            {
                tempTransformRot.y = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting RotY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 13) // Line 13 - RotZ
        {
            try
            {
                tempTransformRot.z = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting RotZ value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 14) // Line 14 - ScaleX
        {
            try
            {
                tempTransformScale.x = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting ScaleX value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 15) // Line 15 - ScaleY
        {
            try
            {
                tempTransformScale.y = std::stof(text);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting ScaleY value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 16) // Line 16 - ScaleZ
        {
            try
            {
                tempTransformScale.z = std::stof(text.c_str());
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error converting ScaleZ value to float: " << e.what() << std::endl;
            }
        }
        else if ((lineCount + objectOffset) % objectSize == 0) // Line 17 - Object ID
        {
            try
            {
                vertices loadedVertices = loadObj(text.c_str());
                add_object(tempID, tempName, loadedVertices, tempIsLight);
                objects.back().transform.pos = tempTransformPos;
                objects.back().transform.scale = tempTransformScale;
                objects.back().transform.rot = tempTransformRot;
                objects.back().texture = tempTexture;

                if (objects.back().light == true) // This can't be applied to everything or else lights break for some reason???
                {
                    objects.back().objectColor = tempLightColor;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error getting object to load: " << e.what() << std::endl;
            }
        }
    }

    std::cout << "File lines: " << lineCount << std::endl;
    std::cout << "Object size: " << objectSize << std::endl;
    std::cout << "Object offset: " << objectOffset << std::endl;
    infile.close();

    return lightCounter;
}

void updateVertices(vertices new_vertices, object object_used) // Future use for animations
{
    std::vector<float> temp_data = convertGLMToOpenGLFLoat(new_vertices);

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i].id == object_used.id)
        {
            objects[i].vertices.id = new_vertices.id;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, object_used.VBO);
    glBufferData(GL_ARRAY_BUFFER, temp_data.size() * sizeof(float), temp_data.data(), GL_STATIC_DRAW);
}

void createWeapon(vertices vertices_used, std::string name, int texture)
{
    weapon newWeapon;
    newWeapon.vertices = vertices_used;
    newWeapon.name = name;
    newWeapon.texture = texture - 1;

    // VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    newWeapon.VAO = VAO;
    newWeapon.VBO = VBO;

    newWeapon.temp_data = convertGLMToOpenGLFLoat(newWeapon.vertices);
    glBufferData(GL_ARRAY_BUFFER, newWeapon.temp_data.size() * sizeof(float), newWeapon.temp_data.data(), GL_STATIC_DRAW);
    int vertexsize = 8;

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    weapons.push_back(newWeapon);

    std::cout << "Created weapon: " << newWeapon.name << std::endl;

    return;
}

#endif // OBJECTS_H