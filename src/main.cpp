#include <stdio.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "shader.h"
// Visual Studio Code is dumb and argues this is an error, ignore this, proceed with make main for no errors

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

uint32_t getTick();

void calculateFrameRate();

bool line_drawing = false;

int main(void)

// Latest: Uniforms
{
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };    
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2
    };



    // Setting up some boring config stuff
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window (width, height, name, monitor, ???)
    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple rendering", NULL, NULL);
    if (window == NULL) // Check if the window was made correctly
    {
        printf("Failed to create window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Set the context to the new window
    gladLoadGL(); // Load glad to stop random segmentation fault

    glViewport(0,0,800,600); // Create a viewport for the new window

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Resize the viewport if the window is resized
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // I'm not even sure
    {
        printf("GLAD failed to initialise\n");
        return -1;
    }

    /* Gets the amount of vertex attributes supported by the GPU
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    */

    // The vertex shader
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // (shader, strings, source code, ???) Compile it or something
    glCompileShader(vertexShader); // Compile the vertex shader

    // Check if the compilation was successful
    int vertexsuccess;
    char vertexinfoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexsuccess);

    if (!vertexsuccess)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, vertexinfoLog); // Put the output into the infolog
        printf("CRITICAL ERROR: Vertex shader was unable to compile\n");
        printf("%s\n",vertexinfoLog);
    }

    // The fragment shader
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0);\n"
        "}\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // (shader, strings, source code, ???) Compile it or something
    glCompileShader(fragmentShader); // Compile the fragment shader

    // Check if the compilation was successful
    int fragmentsuccess;
    char fragmentinfoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentsuccess);

    if (!fragmentsuccess)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, fragmentinfoLog); // Put the output into the infolog
        printf("CRITICAL ERROR: Fragment shader was unable to compile\n");
        printf("%s\n",fragmentinfoLog);
    }


    // Create the shader program (glCreateProgram returns the reference to the new program object)
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // Link the shaders for later use
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int linksuccess;
    char linkinfoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linksuccess);
    if (!linksuccess) // More boring error checking
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, linkinfoLog);
        printf("CRITICAL ERROR: Unable to link shaders\n");
        printf("%s\n",linkinfoLog);
    }

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader); // Delete the shaders since they're already linked they're useless
    glDeleteShader(fragmentShader);  
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    
    
    glBindVertexArray(VAO);
    // Copy the vertices array into a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // Copy the indices to the EBO buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Am I really learning anything if I have no idea what's going on?


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // 1. what vertex attribute is configured (0 is location)
    // 2. the size of the attribute (x,y,z so 3)
    // 3. the type of data parsed
    // 4. if the data should be normalised
    // 5. the strid and which tells OpenGL to go to 3 times the sizse of a float to get to the next
    // piece of data
    // 6. ???

    // Attributes are taken from the current VBO which is bound to the ARRAY_BUFFER
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Bind the colour to the second (1) vertex attribute

    

    glfwSwapInterval(0); // Set to 0 to turn off v-sync


    
    while (!glfwWindowShouldClose(window)) // I want to look at stuff for more than half a second!
    {
        //processInput(window); // Get inputs to do cool things
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear previous colours or else ghosts will haunt this software :ghost: 
        // This is such a stupid comment

        calculateFrameRate();

        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUseProgram(shaderProgram);
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window); // Swap the buffers and poll the events :sunglasses:
        glfwSetKeyCallback(window, key_callback);
        glfwPollEvents();
    }

    glfwTerminate(); // Destroy random stuff for some reason
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }

    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
    {
        if (line_drawing == false)
        {
            line_drawing = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            line_drawing = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

uint32_t getTick()
{
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}


void calculateFrameRate() // If it ain't broke don't fix it
{
    static float framesPerSecond = 0.0f;
    static float lastTime = 0.0f;
    float currentTime = getTick() * 0.001f;
    framesPerSecond++;
    if (currentTime - lastTime > 1.0f)
    {
        lastTime = currentTime;
        float buffer = framesPerSecond;
        printf("fps: %f\n",framesPerSecond);
        framesPerSecond = 0.0f;
    }
}
