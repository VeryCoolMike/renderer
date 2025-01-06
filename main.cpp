#include <stdio.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Visual Studio Code is dumb and argues this is an error, ignore this, proceed with make main for no errors

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

uint32_t getTick();

void calculateFrameRate();

bool line_drawing = false;

unsigned int shaderProgram;

float rotation = 0.0f;
float size = 1.0f;
glm::mat4 trans = glm::mat4(1.0f);

int main(void)

// Latest: Matrix-Vector multiplication
{
    float vertices[] = {
        // positions          // colors           // texture coords
        -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   // top right
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom right
        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,  // bottom left
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 3, 2,
        0, 1, 2,
    };

    // TESTING TODO: DELETE LATER

    
    trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
    trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size



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

    /*
    ███████ ██   ██  █████  ██████  ███████ ██████  ███████ 
    ██      ██   ██ ██   ██ ██   ██ ██      ██   ██ ██      
    ███████ ███████ ███████ ██   ██ █████   ██████  ███████ 
         ██ ██   ██ ██   ██ ██   ██ ██      ██   ██      ██ 
    ███████ ██   ██ ██   ██ ██████  ███████ ██   ██ ███████          
    ANSI REGULAR FOR LARGE COMMENTS                                                                                                  
    */

    // The vertex shader
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        "\n"
        "out vec3 ourColor;\n"
        "out vec2 TexCoord;\n"
        "out float TexID;\n"
        "uniform mat4 transform;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = transform * vec4(aPos, 1.0f);\n"
        "   ourColor = aColor;\n"
        "   TexCoord = aTexCoord;\n"
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
        "\n"
        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"
        "\n"
        "uniform sampler2D texture1;\n"
        "uniform sampler2D texture2;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(texture1, TexCoord);\n"
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

    printf("%i\n", shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1); 
    

    glDeleteShader(vertexShader); // Delete the shaders since they're already linked they're useless
    glDeleteShader(fragmentShader);  
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    
    
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // Copy the indices to the EBO buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Am I really learning anything if I have no idea what's going on?

    

    

    /*
    ███████ ███████ ████████ ██    ██ ██████  
    ██      ██         ██    ██    ██ ██   ██ 
    ███████ █████      ██    ██    ██ ██████  
         ██ ██         ██    ██    ██ ██      
    ███████ ███████    ██     ██████  ██                               
    */

    glfwSwapInterval(1); // Set to 0 to turn off v-sync (You should keep this on)

    int vertexsize = 9; // I hate doing this manually

    // Vertex Attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)0);
    // 1. what vertex attribute is configured (0 is location)
    // 2. the size of the attribute (x,y,z so 3)
    // 3. the type of data parsed
    // 4. if the data should be normalised
    // 5. the strid and which tells OpenGL to go to 3 times the sizse of a float to get to the next
    // piece of data
    // 6. ???

    // Attributes are taken from the current VBO which is bound to the ARRAY_BUFFER
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Bind the colour to the second (1) vertex attribute

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); // Bind the texture to the third (2) vertex attribute
    // Debugging
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    // Textures 
    // https://learnopengl.com/Getting-started/Textures Documentation about textures
    unsigned int texture1;
    glGenTextures(1, &texture1); // Assign the texture an id (1 is the id of the texture)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);  
    unsigned char *data = stbi_load("resources/test.png", &width, &height, &nrChannels, 0);
    
    if (data)
    {
        printf("Image %i loaded successfully!\n", texture1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // 1. Create the image as a 2D texture
        // 2. The mipmap level
        // 3. What format to store the texture in
        // 4. Width
        // 5. Height
        // 6. ??? Set to 0 always
        // 7. The format of the source image (put in the same as how you loaded it)
        // 8. The datatype of the source image (put in the same as how you loaded it)
        // 9. The image data

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load image %i\n!",texture1);
    }

    stbi_image_free(data); // This is freeing the stbi image not the OpenGL texture


    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("resources/awesomeface.png", &width, &height, &nrChannels, 0);
    
    if (data)
    {
        printf("Image %i loaded successfully!\n", texture2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load image %i\n!",texture2);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    stbi_image_free(data); // This is freeing the stbi image not the OpenGL texture


    /*                                   
    ██████  ███████ ███    ██ ██████  ███████ ██████  ██ ███    ██  ██████  
    ██   ██ ██      ████   ██ ██   ██ ██      ██   ██ ██ ████   ██ ██       
    ██████  █████   ██ ██  ██ ██   ██ █████   ██████  ██ ██ ██  ██ ██   ███ 
    ██   ██ ██      ██  ██ ██ ██   ██ ██      ██   ██ ██ ██  ██ ██ ██    ██ 
    ██   ██ ███████ ██   ████ ██████  ███████ ██   ██ ██ ██   ████  ██████                                                              
    */

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    // 1. The uniform's location
    // 2. How many matrices are being sent
    // 3. If the matrix should be transposed (not with GLM)
    // 4. The matrix data (convert with glm::value_ptr because we're using GLM and OpenGL)
    
    while (!glfwWindowShouldClose(window)) // Make the window not immediately close
    {
        //processInput(window); // Get inputs to do cool things
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen to remove ghosting

        calculateFrameRate();



        glUseProgram(shaderProgram);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window); // Swap the buffers and poll the events :sunglasses:
        
        glfwSetKeyCallback(window, key_callback);
        glfwPollEvents();
    }

    glfwTerminate(); // Clean things up!
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

    if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
    {
        size += 0.1f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    {
        size -= 0.1f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
    {
        rotation -= 5.0f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
    {
        rotation += 5.0f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
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
        printf("fps: %f\n",framesPerSecond);
        framesPerSecond = 0.0f;
    }
}