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
#include "include/user_made/render.h"

uint32_t getTick();

float calculateFrameRate();

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

int currentWidth;
int currentHeight;

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

glm::mat4 view = glm::mat4(1.0f);

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
    weapons.back().shotgun = true;

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
    Shader screenShader("shaders/screenshader.vs", "shaders/screenshader.fs");

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
    glm::mat4 proj = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
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

    // Blending for transparent textures
    glEnable(GL_BLEND);  

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // Cull faces that aren't visible
    glEnable(GL_CULL_FACE);

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
    screenShader.setInt("screenTexture", 16);

    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Generate the texture
    
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
        std::cout << error("ERROR: FRAMEBUFFER is not complete!") << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window)) // Render loop
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
        
        // Actual rendering
        render(regularShader, lightShader, screenShader);

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
