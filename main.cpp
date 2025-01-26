#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sstream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "include/user_made/objects.h"
#include "include/user_made/inputHandling.h"
#include "include/user_made/shader.h"
#include "include/user_made/textures.h"
#include "include/user_made/gui.h"

uint32_t getTick();

float calculateFrameRate();

float rotation = 0.0f;
float size = 1.0f;

glm::mat4 trans = glm::mat4(1.0f);

float lastFrame = 0.0f;

float fov = 90.0f;

glm::vec3 lightcolor = glm::vec3(1.0f, 0.0f, 0.0f);

float ambientintensity = 0.1f;

glm::vec3 lightPos(10.0f, 20.0f, 0.0f);

float specularStrength = 0.5;

float lastFPS = 0.0f;

GLfloat backgroundColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};

float currentFrame;

glm::mat4 proj;
glm::mat4 view;
glm::mat4 model;

std::vector<gui> guisVisible;

int fileCount = 0;

// Loading files
vertices coneObj = loadObj("resources/models/cone.obj");
vertices dbShotgun = loadObj("resources/models/shotgun.obj");
vertices skull = loadObj("resources/models/skull.obj");
vertices cubeObj = loadObj("resources/models/cube.obj");

player playerInstance;

std::vector<GLuint> textureArray;

int main(void)
{

    printf("One must imagine sisyphus happy\n");

    // Setting up some boring config stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window (width, height, name, monitor, ???)
    GLFWwindow *window = glfwCreateWindow(1920, 1080, "A little more complex rendering", NULL, NULL);
    if (window == NULL) // Check if the window was made correctly
    {
        std::cout << error("Failed to create window!\n") << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Set the context to the new window
    gladLoadGL();                   // Load glad to stop random segmentation fault

    glViewport(0, 0, 1920, 1080); // Create a viewport for the new window

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the viewport if the window is resized

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // I'm not even sure
    {
        std::cout << error("GLAD failed to initialise\n") << std::endl;
        return -1;
    }

    createWeapon(dbShotgun, "Shotgun", 8);

    /*
    ███████ ██   ██  █████  ██████  ███████ ██████  ███████
    ██      ██   ██ ██   ██ ██   ██ ██      ██   ██ ██
    ███████ ███████ ███████ ██   ██ █████   ██████  ███████
         ██ ██   ██ ██   ██ ██   ██ ██      ██   ██      ██
    ███████ ██   ██ ██   ██ ██████  ███████ ██   ██ ███████
    ANSI REGULAR FOR LARGE COMMENTS
    */
    Shader lightShader("shaders/lights.vs", "shaders/lights.fs");
    Shader regularShader("shaders/regular.vs", "shaders/regular.fs");

    regularShader.use();

    regularShader.setFloat3("lightColor", lightcolor[0], lightcolor[1], lightcolor[2]);
    regularShader.setFloat("ambientStrength", ambientintensity);

    regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

    /*
    ███████ ███████ ████████ ██    ██ ██████
    ██      ██         ██    ██    ██ ██   ██
    ███████ █████      ██    ██    ██ ██████
         ██ ██         ██    ██    ██ ██
    ███████ ███████    ██     ██████  ██
    */

    glfwSwapInterval(0); // Set to 0 to turn off v-sync (You should keep this on)

    glEnable(GL_DEPTH_TEST); // Turn off for no depth test

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable the cursor

    // Debugging
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n", cwd);
    }
    else
    {
        std::cout << error("getcwd() error") << std::endl;
    }

    // Textures
    // https://learnopengl.com/Getting-started/Textures Documentation about textures
    // I thought I could only have 32 textures??? Nope, unlimited??? How does this work, I don't know I thought the max for GL_TEXTURE[?] was 32. Wacky!

    for (auto i : std::filesystem::directory_iterator("resources/textures"))
    {
        if (std::filesystem::is_regular_file(i))
        {
            textureArray.push_back(loadTexture(i.path().string().c_str()));
            std::cout << i << " - " << textureArray.size() << std::endl;
            glActiveTexture(GL_TEXTURE0 + fileCount + 1);
            glBindTexture(GL_TEXTURE_2D, textureArray[fileCount]);
            fileCount++;
        }
    }
    printf("%i textures found!\n", fileCount);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    createGui(window);

    /*
    ██████  ███████ ███    ██ ██████  ███████ ██████  ██ ███    ██  ██████
    ██   ██ ██      ████   ██ ██   ██ ██      ██   ██ ██ ████   ██ ██
    ██████  █████   ██ ██  ██ ██   ██ █████   ██████  ██ ██ ██  ██ ██   ███
    ██   ██ ██      ██  ██ ██ ██   ██ ██      ██   ██ ██ ██  ██ ██ ██    ██
    ██   ██ ███████ ██   ████ ██████  ███████ ██   ██ ██ ██   ████  ██████
    */

    regularShader.setMatrix4fv("transform", 1, GL_FALSE, glm::value_ptr(trans));

    lightShader.setMatrix4fv("transform", 1, GL_FALSE, glm::value_ptr(trans));
    // 1. The uniform's location
    // 2. How many matrices are being sent
    // 3. If the matrix should be transposed (not with GLM)
    // 4. The matrix data (convert with glm::value_ptr because we're using GLM and OpenGL)

    // Get the camera pos, forward, up, and right (up and cameraUp are not the same)

    // Create the perspective projection
    glm::mat4 proj = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 1000.0f);
    regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

    add_object(currentIDNumber, "floor", cubeObj, false);
    objects[currentIDNumber - 1].transform.pos = glm::vec3(0.0f, -5.0f, 0.0f);
    objects[currentIDNumber - 1].transform.scale = glm::vec3(100.0f, 1.0f, 100.0f);

    add_object(currentIDNumber, "box", cubeObj, false);

    for (int n = 0; n < objects.size(); ++n) // Use objects.size() instead of currentIDNumber
    {
        if (objects[n].enabled == false)
        {
            continue;
        }
        gui newGui;
        newGui.id = n;
        newGui.visible = false;
        guisVisible.push_back(newGui);
    }

    int counter = 0;
    for (int i = 0; i < currentIDNumber; i++)
    {
        if (objects[i].enabled == false)
        {
            continue;
        }
        const auto &obj = objects[i];
        if (obj.light == true)
        {
            counter += 1;
        }
    }
    regularShader.setInt("lightAmount", counter); // Avoiding running expensive operations every frame

    while (!glfwWindowShouldClose(window)) // Make the window not immediately close
    {
        glfwMakeContextCurrent(window);

        glfwPollEvents();
        processInput(window); // Get inputs to do cool things
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen to remove ghosting

        float framesPerSecond = calculateFrameRate();

        if (framesPerSecond >= 0.0)
        {
            lastFPS = framesPerSecond;
            printf("%f\n", lastFPS);
        }

        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //  Camera stuff
        // https://learnopengl.com/Getting-started/Camera Euler Angles
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        // direction.y = 0.0f;
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        glm::mat4 view = glm::mat4(1.0f);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        if (!levelEditing)
        {
            direction.y = 0.0f;
        }

        regularShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
        lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));

        for (int i = 0; i < lightArray.size(); i++) // Ah yes, peak, run a for loop every single frame multiple times, truly the pythonic way.
        {
            lightArray[i].pos = objects[lightArray[i].id].transform.pos;
            std::string uniformName = "pointLights[" + std::to_string(i) + "]";
            regularShader.setFloat3(uniformName + ".position", lightArray[i].pos.x, lightArray[i].pos.y, lightArray[i].pos.z);
            regularShader.setBool(uniformName + ".enabled", lightArray[i].enabled);
            regularShader.setFloat3(uniformName + ".color", lightArray[i].color[0], lightArray[i].color[1], lightArray[i].color[2]);
        }

        for (unsigned int i = 0; i < objects.size(); i++)
        {
            if (objects[i].enabled == false)
            {
                continue;
            }
            const auto &obj = objects[i];

            if (obj.light)
            {
                lightShader.use();
                for (int i = 0; i < lightArray.size(); i++) // Ah yes, peak, run a for loop every single frame multiple times, truly the pythonic way.
                {
                    lightArray[i].color = objects[lightArray[i].id].objectColor;
                    if (lightArray[i].id == obj.id)
                    {
                        lightShader.setFloat3("lightColor", lightArray[i].color[0], lightArray[i].color[1], lightArray[i].color[2]);
                    }
                }
                lightShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
                lightShader.setInt("currentTexture", obj.texture);
            }
            else
            {
                regularShader.use();

                regularShader.setInt("currentTexture", obj.texture);
                if (obj.selected == true)
                {
                    regularShader.setBool("selected", true);
                }
                else
                {
                    regularShader.setBool("selected", false);
                }

                regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
                regularShader.setFloat3("objectColor", obj.objectColor[0], obj.objectColor[1], obj.objectColor[2]);
                regularShader.setBool("fullBright", fullBright);
                regularShader.setFloat("ambientStrength", ambientintensity); // Adjust this value as needed
            }

            glBindVertexArray(objects[i].VAO);

            // Transformations
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj.transform.pos);
            model = glm::scale(model, glm::vec3(obj.transform.scale));
            glm::vec3 angle = obj.transform.rot;
            model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));

            if (obj.light == true)
            {
                glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            }
            else
            {
                glUniformMatrix4fv(glGetUniformLocation(regularShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            }

            glDrawArrays(GL_TRIANGLES, 0, obj.temp_data.size());
        }

        // Draw viewmodel

        // Disable despth testing causes strange issues
        regularShader.use();
        // Assuming that the weapon exists, add error checking!
        int currentWeapon = playerInstance.weaponID;

        if (currentWeapon + 1 <= weapons.size() && currentWeapon >= 0) // One weapon would mean size 1 but position 0
        {
            regularShader.setInt("currentTexture", weapons[currentWeapon].texture);

            regularShader.setBool("selected", false);
            regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
            regularShader.setFloat3("objectColor", weapons[currentWeapon].objectColor[0], weapons[currentWeapon].objectColor[1], weapons[currentWeapon].objectColor[2]);
            regularShader.setBool("fullBright", fullBright);
            regularShader.setFloat("ambientStrength", ambientintensity); // Adjust this value as needed

            glBindVertexArray(weapons[currentWeapon].VAO);

            // Transformations
            weapons[currentWeapon].transform.pos = cameraPos + weapons[currentWeapon].offset.pos;
            weapons[currentWeapon].transform.rot.y = -yaw;
            weapons[currentWeapon].transform.rot.z = pitch;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cameraPos); // Make it go to camerapos
            model = glm::scale(model, glm::vec3(weapons[currentWeapon].transform.scale));
            glm::vec3 angle = weapons[currentWeapon].transform.rot;
            model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, weapons[currentWeapon].offset.pos);

            glUniformMatrix4fv(glGetUniformLocation(regularShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, weapons[currentWeapon].temp_data.size());
        }
        else
        {
            if (currentWeapon + 1 > weapons.size()) // Get out of errors
            {
                playerInstance.weaponID = weapons.size() - 1;
            }
            if (currentWeapon < 0)
            {
                playerInstance.weaponID = 0;
            }

            std::cout << error("Current weapon index out of range") << std::endl;
        }

        // Needs to be here for some reason or light vertex shader will stop working?????
        lightShader.use();

        lightShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
        lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
        lightShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

        regularShader.use();

        // Update things
        glm::mat4 proj = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 1000.0f);
        regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

        regularShader.setFloat("ambientStrength", ambientintensity);

        // Render gui
        renderGui(window, regularShader);

        glfwSwapBuffers(window); // Swap the buffers and poll the events :sunglasses:
    }
    // !-X-X-X-X-X-! END OF LOOP !-X-X-X-X-X-!

    printf("Exiting\n");

    glfwTerminate(); // Clean things up!
    for (int i = 0; i < objects.size(); i++)
    {
        glDeleteVertexArrays(1, &objects[i].VAO);
        glDeleteBuffers(1, &objects[i].VBO);
    }
    for (int i = 0; i < fileCount; i++)
    {
        glDeleteTextures(1, &textureArray[i]);
    }
    glDeleteProgram(lightShader.ID);
    glDeleteProgram(regularShader.ID);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
    return 0;
}

uint32_t getTick()
{
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime(CLOCK_REALTIME, &ts);
    theTick = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}

float calculateFrameRate() // If it ain't broke don't fix it
{
    static float framesPerSecond = 0.0f;
    static float lastTime = 0.0f;
    float currentTime = getTick() * 0.001f;
    framesPerSecond++;
    if (currentTime - lastTime > 0.2f)
    {
        lastTime = currentTime;
        float buffer = framesPerSecond * 5;
        framesPerSecond = 0.0f;
        return buffer;
    }
    return -1.0f;
}
