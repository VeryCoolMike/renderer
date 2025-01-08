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

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <sstream>

#include <vector>

void render_gui();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

uint32_t getTick();

void calculateFrameRate();

void processInput(GLFWwindow *window);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void error(char *string);

bool line_drawing = false;

unsigned int shaderProgram;

float rotation = 0.0f;
float size = 1.0f;
//float camera_speed = 0.03f;
glm::mat4 trans = glm::mat4(1.0f);

// Camera stuff
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 400, lastY = 300;

float yaw = 0.0f;
float pitch = 0.0f;

bool mouselocked = true;

bool firstMouse = true;

float fov = 90.0f;

bool gui_visible = false;

float floorheight = -5.0f;
float floorsize = 100.0f;

float real_camera_speed = 2.5f;

glm::vec3 lightcolor = glm::vec3(1.0f,0.0f,0.0f);

float ambientintensity = 0.1f;

glm::vec3 lightPos(0.0f, 20.0f, 0.0f);

float specularStrength = 0.5;

int main(void)

// Latest: Move cubes
{
    /*
    float vertices[] = {
        // positions          // colors           // texture coords
        -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   // top right
        -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom right
        0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,  // bottom left
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   // top left 
    };
    */

    float vertices[] = {
        // Position         // Texture Coords // Normals
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,      0.0f,  1.0f,  0.0f
    };


    float floor[] = {
        // Format for each vertex: position (3), texcoord (2), normal (3)
        // First triangle
        -floorsize, floorheight, -floorsize,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
        floorsize,  floorheight, -floorsize,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
        -floorsize, floorheight,  floorsize,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // top left
        
        // Second triangle
        floorsize,  floorheight, -floorsize,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom right
        floorsize,  floorheight,  floorsize,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // top right
        -floorsize, floorheight,  floorsize,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f   // top left
    };


    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
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
    const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "layout (location = 2) in vec3 aNormal;\n"
    "\n"
    "out vec2 TexCoord;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "uniform mat4 transform;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * transform * vec4(aPos, 1.0));\n"
    "   Normal = mat3(transpose(inverse(model * transform))) * aNormal;\n"
    "   gl_Position = projection * view * model * transform * vec4(aPos, 1.0f);\n"
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
    const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec2 TexCoord;\n"
    "in vec3 Normal;\n"
    "in vec3 FragPos;\n"
    "\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "\n"
    "uniform vec3 objectColor;\n"
    "uniform vec3 lightColor;\n"
    "uniform float ambientStrength;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 viewPos;\n"
    "void main()\n"
    "{\n"
    "   vec3 norm = normalize(Normal);\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   vec3 viewDir = normalize(viewPos - FragPos);\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);\n"
    "   float shininess = 32.0f;\n"
    "   float intensity = 0.5f;\n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
    "   vec3 specular = intensity * spec * lightColor;\n"
    "   \n"
    "   vec3 ambient = ambientStrength * lightColor;\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = diff * lightColor;\n"
    "   vec3 result = (ambient + diffuse + specular) * vec3(1.0f, 1.0f, 1.0f);\n"
    "   FragColor = texture(texture2, TexCoord) * vec4(result, 1.0);\n"
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
        error("CRITICAL ERROR: Fragment shader was unable to compile\n");
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
        error("CRITICAL ERROR: Unable to link shaders\n");
        printf("%s\n",linkinfoLog);
    }


    // !----------! LIGHTING !----------!

    const char *lightVertexShaderSource = // VERTEX
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
    "}\0";

    unsigned int lightVertShader;
    lightVertShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(lightVertShader, 1, &lightVertexShaderSource, NULL); // (shader, strings, source code, ???) Compile it or something
    glCompileShader(lightVertShader); // Compile the light shader

    // Check if the compilation was successful
    int lightvertsuccess;
    char lightvertinfolog[512];
    glGetShaderiv(lightVertShader, GL_COMPILE_STATUS, &lightvertsuccess);

    if (!lightvertsuccess)
    {
        glGetShaderInfoLog(lightVertShader, 512, NULL, lightvertinfolog); // Put the output into the infolog
        error("CRITICAL ERROR: Light Fragment shader was unable to compile\n");
        printf("%s\n",lightvertinfolog);
    }


    const char *lightFragmentShaderSource = // FRAGMENT
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 lightColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(lightColor, 1.0);\n"
    "}\0";

    unsigned int lightFragShader;
    lightFragShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(lightFragShader, 1, &lightFragmentShaderSource, NULL); // (shader, strings, source code, ???) Compile it or something
    glCompileShader(lightFragShader); // Compile the light shader

    // Check if the compilation was successful
    int lightfragsuccess;
    char lightfraginfolog[512];
    glGetShaderiv(lightFragShader, GL_COMPILE_STATUS, &lightfragsuccess);

    if (!lightfragsuccess)
    {
        glGetShaderInfoLog(lightFragShader, 512, NULL, lightfraginfolog); // Put the output into the infolog
        error("CRITICAL ERROR: Light Vertex shader was unable to compile\n");
        printf("%s\n",lightfraginfolog);
    }


   
    // Create the light shader program
    unsigned int lightProgram = glCreateProgram();

    // Link the vertex shader and light fragment shader
    glAttachShader(lightProgram, lightVertShader);  // Reuse the same vertex shader
    glAttachShader(lightProgram, lightFragShader);
    glLinkProgram(lightProgram);

    // Check for linking errors
    int lightProgramSuccess;
    char lightProgramInfoLog[512];
    glGetProgramiv(lightProgram, GL_LINK_STATUS, &lightProgramSuccess);
    if (!lightProgramSuccess) 
    {
        glGetProgramInfoLog(lightProgram, 512, NULL, lightProgramInfoLog);
        error("CRITICAL ERROR: Unable to link light shader program\n");
        printf("%s\n", lightProgramInfoLog);
    }


    glUseProgram(shaderProgram);

    printf("%i\n", shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1); 

    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightcolor[0], lightcolor[1], lightcolor[2]); 
    glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientintensity); 

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 

    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z); 

    glDeleteShader(vertexShader); // Delete the shaders since they're already linked they're useless
    glDeleteShader(fragmentShader);  

    int vertexsize = 8; // I hate doing this manually
    
    // For your main object
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    // Your existing vertex attribute setup for main object
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light cube VAO setup
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);


    // Bind the same VBO since we're using the same vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Configure the light's vertex attributes - note the stride matches your vertex format
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsize * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    

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
    glGenTextures(2, &texture2); // Assign the texture an id (1 is the id of the texture)
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);  
    data = stbi_load("resources/prototype.jpg", &width, &height, &nrChannels, 0);
    
    if (data)
    {
        printf("Image %i loaded successfully!\n", texture2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load image %i\n!",texture2);
    }

    stbi_image_free(data); // This is freeing the stbi image not the OpenGL texture

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
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
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.FontGlobalScale = 1.5f;
    
    // Restore main window context
    glfwMakeContextCurrent(window);

    /*                                   
    ██████  ███████ ███    ██ ██████  ███████ ██████  ██ ███    ██  ██████  
    ██   ██ ██      ████   ██ ██   ██ ██      ██   ██ ██ ████   ██ ██       
    ██████  █████   ██ ██  ██ ██   ██ █████   ██████  ██ ██ ██  ██ ██   ███ 
    ██   ██ ██      ██  ██ ██ ██   ██ ██      ██   ██ ██ ██  ██ ██ ██    ██ 
    ██   ██ ███████ ██   ████ ██████  ███████ ██   ██ ██ ██   ████  ██████                                                              
    */

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    transformLoc = glGetUniformLocation(lightProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    // 1. The uniform's location
    // 2. How many matrices are being sent
    // 3. If the matrix should be transposed (not with GLM)
    // 4. The matrix data (convert with glm::value_ptr because we're using GLM and OpenGL)

    // Get the camera pos, forward, up, and right (up and cameraUp are not the same)
    

    // Create the perspective projection
    glm::mat4 proj = glm::perspective(glm::radians(fov), (float)800/(float)600, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    

    while (!glfwWindowShouldClose(window)) // Make the window not immediately close
    {
        glfwMakeContextCurrent(window);
        
        
        glfwPollEvents();
        processInput(window); // Get inputs to do cool things
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen to remove ghosting

        calculateFrameRate();

        float timeValue = glfwGetTime();
        float lightHeight = (sin(timeValue)) * 10;
        lightPos.y = lightHeight;

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_DYNAMIC_DRAW);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glUseProgram(shaderProgram);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

        // Camera stuff
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
        //printf("%f\n",cameraPos);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        //printf("%f\n",vertices[0]);

        
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3, 0.55));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Bind the buffers to the floor array I can't do this right now im too tired
        //glBindBuffer

        // Floor drawing
        glm::mat4 model = glm::mat4(1.0f);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glUseProgram(lightProgram);
        glUniform3f(glGetUniformLocation(lightProgram, "lightColor"), 1.0f, 0.0f, 0.0f); 
        glBindVertexArray(lightVAO);  // Use the light VAO

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, lightHeight, -5.0f));
        model = glm::scale(model, glm::vec3(2.0f));  // Make the light cube smaller like in the example

        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

        glDrawArrays(GL_TRIANGLES, 0, 36);


        glUseProgram(shaderProgram);

        // Update things
        
        glm::mat4 proj = glm::perspective(glm::radians(fov), (float)800/(float)600, 0.1f, 1000.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

        // Update light
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z); 

        // Update the Uniforms for ImGui and dynamic lighting
        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), ambientintensity); 
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), lightcolor[0], lightcolor[1], lightcolor[2]); 

        floor[0]  = -floorsize; floor[1]  = floorheight; floor[2]  = -floorsize; floor[3]  =  0.0f; floor[4]  =  0.0f;  // vertex 1
        floor[5]  = floorsize;  floor[6]  = floorheight; floor[7]  = -floorsize; floor[8]  =  1.0f; floor[9]  =  0.0f;  // vertex 2
        floor[10] = -floorsize; floor[11] = floorheight; floor[12] = floorsize; floor[13] =  1.0f; floor[14] =  1.0f;  // vertex 3

        floor[15] = floorsize;  floor[16] = floorheight; floor[17] = -floorsize; floor[18] =  0.0f; floor[19] =  0.0f;  // vertex 4
        floor[20] = floorsize;  floor[21] = floorheight; floor[22] = floorsize; floor[23] =  1.0f; floor[24] =  1.0f;  // vertex 5
        floor[25] = -floorsize; floor[26] = floorheight; floor[27] = floorsize; floor[28] =  1.0f; floor[29] =  1.0f;  // vertex 6

        // !-X-X-X-X-X-! GUI !-X-X-X-X-X-!
        if (gui_visible)
        {
            firstMouse = true;
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(400, 300));
            
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

            ImGui::InputFloat("Floor Height", &floorheight, 0.5f);
            ImGui::InputFloat("Camera Speed", &real_camera_speed, 0.1f);

            ImGui::InputFloat("Ambient Strength", &ambientintensity, 0.05f);
            ImGui::InputFloat("Light Color 1", &lightcolor[0]);
            ImGui::InputFloat("Light Color 2", &lightcolor[1]);
            ImGui::InputFloat("Light Color 3", &lightcolor[2]);

            float buf[10][3];

            for (int n = 0; n < 10; n++)
            {
                
                char strbuf[128];
                ImGui::InputFloat3(std::to_string(n).c_str(), &cubePositions[n].x);
            }

            
            ImGui::End();
            

            // ImGui::ShowDemoWindow();
            
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            // glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // GUI background color
            // glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        }
        else
        {
            
            // Ensure ImGui doesn't block input when hidden
            ImGui::GetIO().WantCaptureMouse = false;
            ImGui::GetIO().WantCaptureKeyboard = false;
        }

        


        glfwSwapBuffers(window); // Swap the buffers and poll the events :sunglasses:
    }
    // !-X-X-X-X-X-! END OF LOOP !-X-X-X-X-X-!
 

    glfwTerminate(); // Clean things up!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
    glDeleteProgram(lightProgram);
    glDeleteProgram(shaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    
    ImGui::DestroyContext();
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

    if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
    {
        if (gui_visible == false)
        {
            gui_visible = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Enable the cursor
            mouselocked = false;
        }
        else
        {
            gui_visible = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable the cursor
            mouselocked = true;
        }
    }

    if (key == GLFW_KEY_UP)
    {
        size += 0.1f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    
    }
    if (key == GLFW_KEY_DOWN)
    {
        size -= 0.1f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    }
    if (key == GLFW_KEY_LEFT)
    {
        rotation -= 5.0f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    
    }
    if (key == GLFW_KEY_RIGHT)
    {
        rotation += 5.0f;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(rotation), glm::vec3(0.0,0.0,1.0)); // Rotate theta around Z
        trans = glm::scale(trans, glm::vec3(size, size, size)); // Scale to size
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        printf("%f\n",size);
    }
    if (key == GLFW_KEY_TAB && action == GLFW_RELEASE)
    {
        if (mouselocked == true)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Enable the cursor
            mouselocked = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable the cursor
            mouselocked = true;
        }
    }
}

void processInput(GLFWwindow *window) // This is perfect frame input for things that are held down
{

    float camera_speed = real_camera_speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += camera_speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= camera_speed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camera_speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos += camera_speed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        cameraPos += camera_speed * -cameraUp;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        // Scrolling up
        real_camera_speed += 0.5f;
    }
    else if (yoffset < 0)
    {
        // Scrolling down
        real_camera_speed -= 0.5f;
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


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (!mouselocked)
    {
        firstMouse = true;
        return;
    }
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

void error(char *string)
{
    printf("\033[1;31m%s\033[0m\n", string);
}
