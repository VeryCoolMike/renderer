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

#include "include/user_made/objects.h"
#include "include/user_made/inputHandling.h"
#include "include/user_made/shader.h"
#include "include/user_made/textures.h"
#include "include/user_made/gui.h"
#include "include/user_made/render.h"

uint32_t getTick();

float calculateFrameRate();

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int currentWidth = SCR_WIDTH;
int currentHeight = SCR_HEIGHT;

glm::mat4 trans = glm::mat4(1.0f);

float lastFrame = 0.0f;
float lastFPS = 0.0f;
float currentFrame;

float fov = 90.0f;

std::vector<gui> guisVisible;

int fileCount = 0;

int currentShader = 0;

// Loading files
vertices coneObj = loadObj("resources/models/cone.obj");
vertices dbShotgun = loadObj("resources/models/shotgun.obj");
vertices skull = loadObj("resources/models/skull.obj");
vertices cubeObj = loadObj("resources/models/cube.obj");
vertices sponza = loadObj("resources/models/sponza.obj");

player playerInstance;

std::vector<GLuint> textureArray;

glm::vec3 direction;

unsigned int textureColorbuffer;
unsigned int framebuffer;
unsigned int rbo;

const unsigned int MAX_SHADOWS = 5;

unsigned int depthMapFBOs[MAX_SHADOWS];
unsigned int depthCubeMaps[MAX_SHADOWS];

unsigned int depthDynamicMapFBOs[MAX_SHADOWS];
unsigned int depthDynamicCubeMaps[MAX_SHADOWS];

const unsigned int SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

glm::mat4 view = glm::mat4(1.0f);

float lightFov = 90.0f;

std::vector<glm::vec3> lightPos(MAX_SHADOWS);

int shadowTexture = 30;

bool shadowsEnabled = true;

int shadowCounter = 0;
int shadowDepthCounter = 0;

// Shaders
Shader lightShader;
Shader regularShader;
Shader screenShader;
Shader depthShader;
Shader skyboxShader;

// Camera stuff
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);

/// @brief 
/// @param  
/// @return 
int main(void)
{

    printf("One must imagine sisyphus happy\n");

    // Setting up some boring config stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window (width, height, name, monitor, ???)
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "A little more complex rendering", NULL, NULL);
    if (window == NULL) // Check if the window was made correctly
    {
        std::cout << error("Failed to create window!\n") << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Set the context to the new window
    gladLoadGL();                   // Load glad to stop random segmentation fault

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); // Create a viewport for the new window

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the viewport if the window is resized

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // I'm not even sure
    {
        std::cout << error("GLAD failed to initialise\n") << std::endl;
        return -1;
    }

    // Create weapons
    createWeapon(dbShotgun, "Shotgun", 8);
    weapons.back().shotgun = false;

    /*
    ███████ ██   ██  █████  ██████  ███████ ██████  ███████
    ██      ██   ██ ██   ██ ██   ██ ██      ██   ██ ██
    ███████ ███████ ███████ ██   ██ █████   ██████  ███████
         ██ ██   ██ ██   ██ ██   ██ ██      ██   ██      ██
    ███████ ██   ██ ██   ██ ██████  ███████ ██   ██ ███████
    ANSI REGULAR FOR LARGE COMMENTS
    */

    lightShader = Shader("shaders/lights.vs", "shaders/lights.fs");
    regularShader = Shader("shaders/regular.vs", "shaders/regular.fs");
    screenShader = Shader("shaders/screenShader.vs", "shaders/screenShader.fs");
    depthShader = Shader("shaders/depthShader.vs", "shaders/depthShader.fs", "shaders/depthShader.gs");
    skyboxShader = Shader("shaders/skybox.vs", "shaders/skybox.fs");

    regularShader.use();

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

    //glEnable(GL_STENCIL_TEST); // https://learnopengl.com/Advanced-OpenGL/Stencil-testing

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
    // Edit: turns out i can use 35661, yeah thats enough
    // Edit 2: Figured out that 32 is the maximum texture units i can have activated (Change this later to support for than 32 textures) https://www.reddit.com/r/opengl/comments/o4iryb/32_texture_limit/

    for (auto i : std::filesystem::directory_iterator("resources/textures"))
    {
        if (std::filesystem::is_regular_file(i))
        {
            textureArray.push_back(loadTexture(i.path().string().c_str()));
            std::cout << i << " - " << textureArray.size() << std::endl;
            
            fileCount++;
        }
    }
    printf("%i textures found!\n", fileCount);

    // Load skybox
    std::vector<std::string> faces
    {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg"
    };

    stbi_set_flip_vertically_on_load(false);

    unsigned int cubeMapTexture = loadCubeMap(faces);
    
    std::cout << cubeMapTexture << " texture" << std::endl;

    skyboxShader.use();
    skyboxShader.setInt("skybox", 3);
    regularShader.use();

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    createSkybox();
    createGui(window);

    /*
    ██████  ███████ ███    ██ ██████  ███████ ██████  ██ ███    ██  ██████
    ██   ██ ██      ████   ██ ██   ██ ██      ██   ██ ██ ████   ██ ██
    ██████  █████   ██ ██  ██ ██   ██ █████   ██████  ██ ██ ██  ██ ██   ███
    ██   ██ ██      ██  ██ ██ ██   ██ ██      ██   ██ ██ ██  ██ ██ ██    ██
    ██   ██ ███████ ██   ████ ██████  ███████ ██   ██ ██ ██   ████  ██████
    */

    // Get the camera pos, forward, up, and right (up and cameraUp are not the same, up is the vector that is up on the camera and cameraUp is the world's up)

    add_object(currentIDNumber, "floor", cubeObj, false);
    objects[currentIDNumber - 1].transform.pos = glm::vec3(0.0f, -5.0f, 0.0f);
    objects[currentIDNumber - 1].transform.scale = glm::vec3(100.0f, 1.0f, 100.0f);

    add_object(currentIDNumber, "box", cubeObj, false);

    // Create the perspective projection
    glm::mat4 proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));


    // Blending for transparent textures
    glEnable(GL_BLEND);  

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // Frame buffer https://learnopengl.com/Advanced-OpenGL/Framebuffers

    // Making the quad

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    screenShader.use();
    screenShader.setInt("screenShader", 31);
    screenShader.setInt("depthMap", 30);

    // Frame buffer for full screen shaders
    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << error("ERROR: FRAMEBUFFER (FULLSCREEN SHADER) is not complete!") << std::endl;
    }

    // Frame buffer for shadows

    for (int i = 0; i < MAX_SHADOWS; i++)
    {   

        // Static

        glGenFramebuffers(1, &depthMapFBOs[i]);

        // Create depth map texture
        glGenTextures(1, &depthCubeMaps[i]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMaps[i]);
        for (int j = 0; j < 6; j++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOs[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMaps[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << error("ERROR::FRAMEBUFFER:: Framebuffer for static is not complete!") << std::endl;
        }

        glDrawBuffer(GL_NONE); // To tell OpenGL that there is no colour data
        glReadBuffer(GL_NONE);



        
        // Dynamic
        glGenFramebuffers(1, &depthDynamicMapFBOs[i]);

        // Create depth map texture
        glGenTextures(1, &depthDynamicCubeMaps[i]);

        glBindTexture(GL_TEXTURE_CUBE_MAP, depthDynamicCubeMaps[i]);
        for (int j = 0; j < 6; j++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depthDynamicMapFBOs[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthDynamicCubeMaps[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << error("ERROR::FRAMEBUFFER:: Framebuffer for dynamic is not complete!") << std::endl;
        }

        glDrawBuffer(GL_NONE); // To tell OpenGL that there is no colour data
        glReadBuffer(GL_NONE);
        
    }

    updateStaticShadows();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window)) // !---!---! RENDER LOOP !---!---!
    {
        glfwMakeContextCurrent(window);

        glfwPollEvents();
        processInput(window); // Get inputs to do cool things
        

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
        
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        // direction.y = 0.0f;
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        view = glm::mat4(1.0f);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        if (!levelEditing)
        {
            direction.y = 0.0f;
        }

        lightPos.clear();

        for (int i = 0; i < lightArray.size(); i++)
        {
            if (lightArray[i].castShadow == true)
            {
                lightArray[i].shadowID = lightPos.size(); // Wowzers
                //std::cout << lightArray[i].shadowID << std::endl;
                lightPos.push_back(lightArray[i].pos);
                
            }
        }

        //updateDynamicShadows();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // Actual rendering
        glViewport(0, 0, currentWidth, currentHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        screenShader.use();
        screenShader.setInt("shader", currentShader);
        screenShader.setInt("screenTexture", 31);

        regularShader.use();
        
        //std::cout << currentFrame * 100000 << std::endl;
        //if (static_cast<int> (currentFrame * 100000) % 42 == 0)
        //{
        //    int shadowIndex = 0;
        //    updateShadows(&shadowIndex);
        //}


        // First pass
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

        render();

        // Weapons
        int currentWeapon = playerInstance.weaponID;

        if (currentWeapon + 1 <= weapons.size() && currentWeapon >= 0) // One weapon would mean size 1 but position 0
        {
            renderWeapon(currentWeapon);
        }
        else
        {
            if (currentWeapon + 1 > weapons.size()) // Incase the current weapon is somehow out of range
            {
                playerInstance.weaponID = weapons.size() - 1;
            }
            if (currentWeapon < 0)
            {
                playerInstance.weaponID = 0;
            }

            std::cout << error("Current weapon index out of range") << std::endl;
        }

        renderSkybox(cubeMapTexture);

        // Second pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        screenShader.setInt("shader", currentShader);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE31);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteProgram(lightShader.ID);
    glDeleteProgram(regularShader.ID);
    glDeleteProgram(screenShader.ID);
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
