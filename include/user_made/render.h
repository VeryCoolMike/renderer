#ifndef RENDER_H
#define RENDER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "structs.h"
#include "helper.h"



float ambientintensity = 0.1f;

GLfloat backgroundColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};

glm::mat4 proj;
extern glm::mat4 view;
glm::mat4 model;

unsigned int skyboxVAO;
unsigned int skyboxVBO;

extern unsigned int textureColorbuffer;
extern unsigned int framebuffer;
extern unsigned int rbo;

extern unsigned int depthMapFBOs[5];
extern unsigned int depthCubeMaps[5];
extern unsigned int depthDynamicMapFBOs[5];
extern unsigned int depthDynamicCubeMaps[5];

extern glm::vec3 cameraPos;

extern int currentShader;

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;
extern const unsigned int SHADOW_WIDTH;
extern const unsigned int SHADOW_HEIGHT;

extern float fov;
extern float yaw;
extern float pitch;

extern player playerInstance;

extern std::vector<glm::vec3> lightPos;

extern std::vector<GLuint> textureArray;

extern bool shadowsEnabled;

extern int currentWidth;
extern int currentHeight;

// Shaders
extern Shader lightShader;
extern Shader regularShader;
extern Shader screenShader;
extern Shader depthShader;
extern Shader skyboxShader;


// Camera
extern glm::vec3 cameraPos;
extern glm::vec3 cameraTarget;
extern glm::vec3 cameraDirection;

extern glm::vec3 up;
extern glm::vec3 cameraRight;
extern glm::vec3 cameraUp;
extern glm::vec3 cameraFront;


void render()
{

    proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    lightShader.use();
    lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    regularShader.use();
    regularShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    
    regularShader.use();

    for (int i = 0; i < lightArray.size(); i++) // Ah yes, peak, run a for loop every single frame multiple times, truly the pythonic way.
    {
        lightArray[i].pos = objects[lightArray[i].id].transform.pos;
        std::string uniformName = "pointLights[" + std::to_string(i) + "]";
        regularShader.setFloat3(uniformName + ".position", lightArray[i].pos.x, lightArray[i].pos.y, lightArray[i].pos.z);
        regularShader.setBool(uniformName + ".enabled", lightArray[i].enabled);
        regularShader.setFloat3(uniformName + ".color", lightArray[i].color[0], lightArray[i].color[1], lightArray[i].color[2]); // For some reason red
        regularShader.setFloat(uniformName + ".strength", lightArray[i].strength);
        regularShader.setFloat(uniformName + ".castShadow", lightArray[i].castShadow);
        if (lightArray[i].castShadow == true)
        {
            regularShader.setInt(uniformName + ".shadowID", lightArray[i].shadowID);
        }
        
    }

    regularShader.setBool("shadowsEnabled", shadowsEnabled);

    regularShader.setInt("lightAmount", lightArray.size()); // Me when no access to regular shader :moyai:
    regularShader.setFloat("ambientStrength", ambientintensity);
    regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
    regularShader.setInt("skybox", 3);

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        if (objects[i].enabled == false || objects[i].visible == false)
        {
            continue;
        }
        const auto &obj = objects[i];

        // Textures
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureArray[obj.texture]);

        // Transformations
        model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.pos);
        model = glm::scale(model, glm::vec3(obj.transform.scale));
        glm::vec3 angle = obj.transform.rot;
        model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));

        if (obj.light)
        {
            lightShader.use();
            for (int j = 0; j < lightArray.size(); j++) // Ah yes, peak, run a for loop every single frame multiple times, truly the pythonic way.
            {
                
                lightArray[j].color = objects[lightArray[j].id].objectColor;
                if (lightArray[j].id == obj.id)
                {
                    lightShader.setFloat3("lightColor", lightArray[j].color[0], lightArray[j].color[1], lightArray[j].color[2]);
                    break;
                }
            }
            lightShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));

            lightShader.setInt("currentTexture", 1);
        }
        else
        {
            regularShader.use();
            
            regularShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            regularShader.setBool("selected", obj.selected);
            regularShader.setInt("currentTexture", 1);
            regularShader.setFloat3("objectColor", obj.objectColor[0], obj.objectColor[1], obj.objectColor[2]);
            regularShader.setFloat("reflectancy", obj.reflectance); // Adjust this value as needed
        }

        glBindVertexArray(objects[i].VAO);

        glDrawArrays(GL_TRIANGLES, 0, obj.temp_data.size());
    }

    // Needs to be here for some reason or light vertex shader will stop working?????
    lightShader.use();

    lightShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
    lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    lightShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

    regularShader.use();

    regularShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
    regularShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
    regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));



    // Update things
    //regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

    regularShader.setFloat("ambientStrength", ambientintensity);

    
}

void renderWeapon(int currentWeapon)
{
    regularShader.use();

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, weapons[currentWeapon].texture + 1); // Need the +1 for whatever reason because its being offseted somewhere somehow

    regularShader.setInt("currentTexture", 2);

    regularShader.setBool("selected", false);
    regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
    regularShader.setFloat3("objectColor", weapons[currentWeapon].objectColor[0], weapons[currentWeapon].objectColor[1], weapons[currentWeapon].objectColor[2]);
    regularShader.setFloat("ambientStrength", ambientintensity); // Adjust this value as needed
    regularShader.setFloat("reflectancy", 0.0f); // Adjust this value as needed

    glBindVertexArray(weapons[currentWeapon].VAO);

    // Transformations
    weapons[currentWeapon].transform.pos = cameraPos + weapons[currentWeapon].offset.pos;
    weapons[currentWeapon].transform.rot.y = -yaw;
    weapons[currentWeapon].transform.rot.z = pitch;
    model = glm::mat4(1.0f);
    model = glm::translate(model, cameraPos); // Make it go to camerapos
    model = glm::scale(model, glm::vec3(weapons[currentWeapon].transform.scale));
    glm::vec3 angle = weapons[currentWeapon].transform.rot;
    model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, weapons[currentWeapon].offset.pos);

    regularShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, weapons[currentWeapon].temp_data.size());
}

void renderDepth(int currentMap, bool dynamic = false)
{
    // First pass
    if (dynamic == true)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, depthDynamicMapFBOs[currentMap]);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[currentMap]);
    }
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

    proj = glm::perspective(glm::radians(fov), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 0.1f, 1000.0f);

    depthShader.use();

    for (unsigned int i = 0; i < objects.size(); i++)
    {
        if (objects[i].enabled == false || objects[i].visible == false)
        {
            continue;
        }
        
        if (dynamic == true)
        {
            if (objects[i].dynamic == false)
            {
                continue;
            }
        }
        else
        {
            if (objects[i].dynamic == true)
            {
                continue;
            }
        }
        
        
        const auto &obj = objects[i];

        // Transformations
        model = glm::mat4(1.0f);
        model = glm::translate(model, obj.transform.pos);
        model = glm::scale(model, glm::vec3(obj.transform.scale));
        glm::vec3 angle = obj.transform.rot;
        model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));

        depthShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(objects[i].VAO);

        glDrawArrays(GL_TRIANGLES, 0, obj.temp_data.size());
    }
}

void updateStaticShadows()
{
    
    regularShader.use();

    float near_plane = 1.0f, far_plane = 20.0f;
    regularShader.setFloat("far_plane", far_plane);

    glEnable(GL_CULL_FACE);
    
    for (int i = 0; i < 5; i++) // 5 is MAX_SHADOWS
    {
        // Rendering to depth map
        
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj *
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

        // Rendering to depth cubemap
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        for (int j = 0; j < 6; j++)
        {
            depthShader.setMatrix4fv("shadowMatrices[" + std::to_string(i) + "]" + "[" + std::to_string(j) + "]", 1, GL_FALSE, glm::value_ptr(shadowTransforms[j]));
        }
        depthShader.setFloat("far_plane", far_plane);
        depthShader.setInt("shadow", i);
        depthShader.setFloat3("lightPos["+ std::to_string(i) + "]", lightPos[i].x, lightPos[i].y, lightPos[i].z);
        regularShader.use();
        regularShader.setFloat3("lightPos["+ std::to_string(i) + "]", lightPos[i].x, lightPos[i].y, lightPos[i].z);
        regularShader.setInt("shadowMap[" + std::to_string(i) + "]", 30 - i);
        glActiveTexture(GL_TEXTURE30-i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[i]);

        renderDepth(i, false);
    }

    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWidth, currentHeight);
}

void updateDynamicShadows()
{
    
    regularShader.use();

    float near_plane = 1.0f, far_plane = 20.0f;
    regularShader.setFloat("far_plane", far_plane);

    glEnable(GL_CULL_FACE);
    
    for (int i = 0; i < 5; i++) // 5 is MAX_SHADOWS
    {
        // Rendering to depth map
        
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj *
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * 
                    glm::lookAt(lightPos[i], lightPos[i] + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

        // Rendering to depth cubemap
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthDynamicMapFBOs[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.use();
        for (int j = 0; j < 6; j++)
        {
            depthShader.setMatrix4fv("shadowMatrices[" + std::to_string(i) + "]" + "[" + std::to_string(j) + "]", 1, GL_FALSE, glm::value_ptr(shadowTransforms[j]));
        }
        depthShader.setFloat("far_plane", far_plane);
        depthShader.setInt("shadow", i);
        depthShader.setFloat3("lightPos["+ std::to_string(i) + "]", lightPos[i].x, lightPos[i].y, lightPos[i].z);
        regularShader.use();
        regularShader.setFloat3("lightPos["+ std::to_string(i) + "]", lightPos[i].x, lightPos[i].y, lightPos[i].z);
        regularShader.setInt("dynamicShadowMap[" + std::to_string(i) + "]", 30 - 6 - i);
        glActiveTexture(GL_TEXTURE30-6-i); // - MAX_SHADOWS to not intrude on the static shadows, i for the shadow id itself and finally - 1 because 30-5-0 is 25 and 30-5 is 25 so they intrude
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthDynamicCubeMaps[i]);

        renderDepth(i, true);
    }

    glDisable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWidth, currentHeight);
}

void createSkybox()
{
    std::vector<float> skyboxVertices = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

void renderSkybox(unsigned int cubeMapTexture)
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    skyboxShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp))))); // Oh the misery
    skyboxShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

#endif // RENDER_H