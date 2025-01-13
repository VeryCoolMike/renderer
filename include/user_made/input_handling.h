void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);


// Camera stuff
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);

bool gui_visible = false;

bool line_drawing = false;

float deltaTime = 0.0f;

float lastX = 400, lastY = 300;

float yaw = 0.0f;
float pitch = 0.0f;

float real_camera_speed = 2.5f;

bool mouselocked = true;

bool firstMouse = true;

bool fullBright = false;

bool duplicateCooldown = false;
std::chrono::time_point<std::chrono::high_resolution_clock> cooldownStart;

std::chrono::time_point<std::chrono::high_resolution_clock> jumpCooldown;


extern std::vector<object> objects;

glm::vec3 movementVector = glm::vec3(0.0f, 0.0f, 0.0f);

bool levelEditing;

float swaySpeed = 5.0f; // Controls sway speed
float swayAmount = 0.00005f; // Controls sway magnitude

bool grounded;

float gravity = 9.81f;

extern float currentFrame;

glm::vec3 targetMovementVector(0.0f);

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

    if (key == GLFW_KEY_T && action == GLFW_RELEASE)
    {
        if (gui_visible == false)
        {
            if (levelEditing == false)
            {
                levelEditing = true;
            }
            else
            {
                levelEditing = false;
                gui_visible = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mouselocked = true;
                cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }
        
    }

    if (key == GLFW_KEY_X && action == GLFW_RELEASE)
    {
        if (fullBright == false)
        {
            fullBright = true;
            printf("Fullbright enabled\n");
        }
        else
        {
            fullBright = false;
        }
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) // This is perfect frame input for things that are held down
{
    if (!levelEditing)
    {
        cameraPos.y -= gravity * deltaTime;

        if (cameraPos.y <= -2.5f) {
            cameraPos.y = -2.5f;
            grounded = true;
        }
        
    }

    float camera_speed = real_camera_speed * deltaTime;
    // Jumping will be worked on later
    /*
    auto now2 = std::chrono::high_resolution_clock::now();
    auto elapsed2 = std::chrono::duration_cast<std::chrono::seconds>(now2 - jumpCooldown).count();
    if (elapsed2 >= 0.3f)
    {
        gravity = 9.81f;
    }
    else
    {
        printf("JUMPING!\n");
        targetMovementVector.y += 15.0f * deltaTime;
    }
    */
    if (gui_visible == false)
    {
        if (levelEditing)
        {
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
        else
        {
            targetMovementVector = glm::vec3(0.0f);
            real_camera_speed = 5.0f;
            cameraFront.y = 0.0f; // Ignore vertical movement
            cameraFront = glm::normalize(cameraFront);

            movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
            
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                targetMovementVector += cameraFront;
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                targetMovementVector -= cameraFront;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                targetMovementVector += glm::normalize(glm::cross(cameraFront, cameraUp));
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                targetMovementVector -= glm::normalize(glm::cross(cameraFront, cameraUp));
            }
            /*
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                if (grounded == true)
                {// Adjust gravity throughout jump
                    //targetMovementVector.y = 150.0f;         // Red (0.0 to 1.0);
                    grounded = false;
                    gravity = 0.0f;
                    jumpCooldown = std::chrono::high_resolution_clock::now();
                }
            }
            */

            //targetMovementVector = glm::normalize(targetMovementVector) * camera_speed;

            // Smooth transition to the target vector
            movementVector = glm::mix(movementVector, targetMovementVector, deltaTime * 5.0f);


            if (glm::length(movementVector) > 0.0f) {
                float sway = std::sin(glfwGetTime() * swaySpeed) * swayAmount;
                cameraPos.y += sway;
            }
            std::cout << movementVector.x << "-" << movementVector.y << "-" << movementVector.z<< std::endl;
            cameraPos += movementVector;
        }
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Duplicate
        {
            if (duplicateCooldown == false)
            {
                duplicateCooldown = true;
                cooldownStart = std::chrono::high_resolution_clock::now();
                for (int i = 0; i < objects.size(); i++)
                {
                    if (objects[i].selected == true)
                    {
                        add_object(currentIDNumber, objects[i].name, cubeVert, objects[i].light);
                        objects.back().transform = objects[i].transform;
                        if (objects[i].light == true)
                        {
                            objects.back().objectColor = objects[i].objectColor;
                        }
                    }
                }
            }
            else
            { // Don't worry about how this works just know it does
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - cooldownStart).count();
                if (elapsed >= 1)
                {
                    duplicateCooldown = false;
                }
            }
        }
    }

    
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!mouselocked)
    {
        return;
    }
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
    //direction.y = sin(glm::radians(pitch));
    direction.y = 0.0f;
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    //cameraFront = glm::normalize(direction);
}  
