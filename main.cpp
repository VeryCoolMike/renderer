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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION


#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "include/user_made/objects.h"
#include "include/user_made/vertices.h"
#include "include/user_made/input_handling.h"
#include "include/user_made/shader.h"
#include "include/user_made/textures.h"

void render_gui();

uint32_t getTick();

float calculateFrameRate();

void error(const char *string);

float rotation = 0.0f;
float size = 1.0f;

glm::mat4 trans = glm::mat4(1.0f);

float lastFrame = 0.0f;

float fov = 90.0f;

glm::vec3 lightcolor = glm::vec3(1.0f, 0.0f, 0.0f);

float ambientintensity = 0.1f;

glm::vec3 lightPos(10.0f, 20.0f, 0.0f);

float specularStrength = 0.5;

bool rainbowMode = false;

float lastFPS = 0.0f;

GLfloat backgroundColor[4] = {0.2f, 0.3f, 0.3f, 1.0f};


// Put pointers inside the objects array which point to the object(s)

int main(void)

// Latest: Issue when adding a light and then destroying it (Does not happen on cubes) Fix: Don't put lights in other objects
{
    std::vector<GLuint> textureArray;
    printf("One must imagine sisyphus happy\n");

    // Setting up some boring config stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window (width, height, name, monitor, ???)
    GLFWwindow *window = glfwCreateWindow(1920, 1080, "Simple rendering", NULL, NULL);
    if (window == NULL) // Check if the window was made correctly
    {
        error("Failed to create window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Set the context to the new window
    gladLoadGL();                   // Load glad to stop random segmentation fault

    glViewport(0, 0, 1920, 1080); // Create a viewport for the new window

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the viewport if the window is resized

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // I'm not even sure
    {
        error("GLAD failed to initialise\n");
        return -1;
    }

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

    regularShader.setFloat3("lightPos", lightPos.x, lightPos.y, lightPos.z);

    regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

    int vertexsize = 8; // I hate doing this manually

    // For your main object
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    // Your existing vertex attribute setup for main object
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light cube VAO setup
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // Bind the same VBO since we're using the same vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Configure the light's vertex attributes - note the stride matches your vertex format
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
        perror("getcwd() error");
    }

    // Textures
    // https://learnopengl.com/Getting-started/Textures Documentation about textures   
    int fileCount = 0;
    for (auto i : std::filesystem::directory_iterator("resources/textures"))
    {
        if (std::filesystem::is_regular_file(i))
        {
            std::cout << i << std::endl;
            textureArray.push_back(loadTexture(i.path().string().c_str()));
            glActiveTexture(GL_TEXTURE0 + fileCount+1);
            glBindTexture(GL_TEXTURE_2D, textureArray[fileCount]);
            fileCount++;
        }
    }
    printf("%i textures found!\n",fileCount);

    for (int i = 0; i < fileCount; i++)
    {
        std::cout << textureArray[i] << std::endl;
    }

    /*
    unsigned int texture1 = loadTexture("resources/textures/glowstone.png");
    unsigned int texture2 = loadTexture("resources/textures/prototype.jpg");


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    */
    /*
     ██████  ██    ██ ██
    ██       ██    ██ ██
    ██   ███ ██    ██ ██
    ██    ██ ██    ██ ██
    ██████   ██████   ██
    */

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.FontGlobalScale = 2.0f;

    // Restore main window context
    glfwMakeContextCurrent(window);

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

    add_object(currentIDNumber, "box", cubeVert, false, regularShader.ID);
    add_object(currentIDNumber, "floor", floorVert, false, regularShader.ID);
    objects[currentIDNumber - 1].transform.pos = glm::vec3(0.0f, -5.0f, 0.0f);

    add_object(currentIDNumber, "reallight", cubeVert, true, lightShader.ID);
    objects[currentIDNumber - 1].transform.pos = glm::vec3(5.0f, 20.0f, 0.0f);
    objects[currentIDNumber - 1].transform.scale = glm::vec3(2.0f, 2.0f, 2.0f);
    objects[currentIDNumber - 1].texture = 0;

    std::string reallightname = objects[currentIDNumber - 1].name;

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
            printf("%f\n",lastFPS);
        }

        float timeValue = glfwGetTime();
        float lightHeight = (sin(timeValue)) * 5 + 2;
        float lightZ = (sin(timeValue)) * 5;
        lightPos.y = lightHeight;
        lightPos.z = lightZ;
        get_object_by_name(reallightname).transform.pos.x = lightPos.x / 2;
        get_object_by_name(reallightname).transform.pos.y = lightPos.y;
        get_object_by_name(reallightname).transform.pos.z = lightPos.z / 2;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size));                       // Scale to size
        regularShader.setMatrix4fv("transform", 1, GL_FALSE, glm::value_ptr(trans));

        //  Camera stuff
        // https://learnopengl.com/Getting-started/Camera Euler Angles
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);

        glm::mat4 view = glm::mat4(1.0f);

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        regularShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
        lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));

        for (unsigned int i = 0; i < currentIDNumber; i++)
        {
            const auto& obj = objects[i];
            glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_DYNAMIC_DRAW);

            //printf("%i\n",obj.texture);

            if (obj.light == true)
            {
                lightShader.use();
                lightShader.setFloat3("lightPos", lightPos.x, lightPos.y, lightPos.z);
                lightShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
                lightShader.setInt("currentTexture", obj.texture);
                glBindVertexArray(lightVAO); // Use the light VAO
            }
            else
            {
                regularShader.use();
                regularShader.setInt("currentTexture", obj.texture);
                regularShader.setFloat3("lightPos", lightPos.x, lightPos.y, lightPos.z);
                regularShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
                regularShader.setFloat3("lightColor", lightcolor[0], lightcolor[1], lightcolor[2]);
                glBindVertexArray(VAO); // Use the VAO
            }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(obj.transform.scale));
            model = glm::translate(model, obj.transform.pos);
            glm::vec3 angle = obj.transform.rot;
            model = glm::rotate(model, glm::radians(angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(angle.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(glGetUniformLocation(objects[i].shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, objects[i].vertices.size() / (vertexsize) * 3);
        }

        // Needs to be here for some reason or light vertex shader will stop working?????
        lightShader.use();
        lightShader.setFloat3("lightColor", lightcolor[0], lightcolor[1], lightcolor[2]);
        
        lightShader.setFloat3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        glBindVertexArray(lightVAO); // Use the light VAO

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, lightHeight, -5.0f));
        model = glm::scale(model, glm::vec3(2.0f)); // Make the light cube smaller like in the example

        lightShader.setMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
        lightShader.setMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
        lightShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

        lightShader.use();
        glBindVertexArray(lightVAO); // Use the light VAO

        regularShader.use();

        // Update things
        glm::mat4 proj = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 1000.0f);
        regularShader.setMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(proj));

        regularShader.setFloat("ambientStrength", ambientintensity);

        if (rainbowMode)
        {
            lightcolor[0] = (sin(timeValue) * 0.5 + 0.5);          // Red (0.0 to 1.0)
            lightcolor[1] = (sin(timeValue + 2.0944) * 0.5 + 0.5); // Green (0.0 to 1.0)
            lightcolor[2] = (sin(timeValue + 4.1888) * 0.5 + 0.5); // Blue (0.0 to 1.0)

            backgroundColor[2] = (sin(timeValue) * 0.5 + 0.5);          // Red (0.0 to 1.0)
            backgroundColor[1] = (sin(timeValue + 2.0944) * 0.5 + 0.5); // Green (0.0 to 1.0)
            backgroundColor[0] = (sin(timeValue + 4.1888) * 0.5 + 0.5); // Blue (0.0 to 1.0)
        }

        // !-X-X-X-X-X-! GUI !-X-X-X-X-X-!
        if (gui_visible)
        {
            firstMouse = true;
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(500, 800));

            ImGui::Begin("Control Panel", nullptr,
                         ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse);

            // Add your GUI controls here
            ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f);
            ImGui::SliderFloat("Size", &size, 0.1f, 5.0f);
            ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.0f);
            ImGui::Text("Camera Position: %.1f, %.1f, %.1f",
                        cameraPos.x, cameraPos.y, cameraPos.z);

            ImGui::Text("FPS: %f", lastFPS);

            if (ImGui::Button("Make Cube")) // Borked, weird interaction when lights exist and spawning regular cubes, but not more lights??? Who knows, Spawning another light fixes????
            {
                add_object(currentIDNumber, "box", cubeVert, false, regularShader.ID);
                objects[currentIDNumber-1].texture = 0;
            }
            if (ImGui::Button("Make Light"))
            {
                add_object(currentIDNumber, "light", cubeVert, true, lightShader.ID);
            }

            ImGui::InputFloat("Camera Speed", &real_camera_speed, 0.1f);

            ImGui::InputFloat("Ambient Strength", &ambientintensity, 0.05f);
            ImGui::ColorPicker3("Light Color", glm::value_ptr(lightcolor));
            ImGui::ColorPicker4("Background COlor", backgroundColor);


            ImGui::Checkbox("Rainbow mode!!!", &rainbowMode);

            for (int n = 0; n < objects.size(); ++n) // Use objects.size() instead of currentIDNumber
            {
                ImGui::PushID(n); // Push the index as the unique ID
                ImGui::DragFloat3((objects[n].name).c_str(), &objects[n].transform.pos.x);
                
                ImGui::Text("ID: %i", objects[n].id);
                for (int i = 0; i < fileCount; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::ImageButton("##texture1", (ImTextureID)(uint64_t)textureArray[i], ImVec2(32, 32), ImVec2(0,0)))  // 0 for no padding
                    {
                        objects[n].texture = i;
                    }
                    ImGui::SameLine();
                    ImGui::PopID();
                }
                ImGui::NewLine();
                
                if (ImGui::Button("Delete"))
                {
                    // Remove the object at index n
                    objects.erase(objects.begin() + n);

                    // Adjust the loop counter because the vector size has changed
                    --n;
                }
                ImGui::PopID(); // Pop the ID after the widget
            }

            ImGui::End();


            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            // glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // GUI background color
            // glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window); // Swap the buffers and poll the events :sunglasses:
    }
    // !-X-X-X-X-X-! END OF LOOP !-X-X-X-X-X-!

    printf("Exitng\n");

    glfwTerminate(); // Clean things up!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

void error(const char *string)
{
    printf("\033[1;31m%s\033[0m\n", string);
}
