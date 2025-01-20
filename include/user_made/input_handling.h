void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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

float swaySpeed = 5.0f;      // Controls sway speed
float swayAmount = 0.00005f; // Controls sway magnitude

bool grounded;

bool ignore_collisions = false;

float gravity = 14.00f;
float originalgravity = gravity;

float height = 2.5f;

extern float currentFrame;

int mouse_x = 0;
int mouse_y = 0;

glm::vec3 targetMovementVector(0.0f);

extern glm::mat4 proj;
extern glm::mat4 view;
extern glm::mat4 model;

const float EPSILON = 0.000001f;
bool debounce = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) // This is perfect frame input for things that are held down
{

    if (!levelEditing)
    {
        if (!grounded)
        {
            cameraPos.y -= gravity * deltaTime;
        }
    }

    float camera_speed = real_camera_speed * deltaTime;

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

    // Playing movement

    if (!levelEditing)
    {
        targetMovementVector = glm::vec3(0.0f);

        real_camera_speed = 5.0f;
        cameraFront.y = 0.0f; // Ignore vertical movement
        cameraFront = glm::normalize(cameraFront);

        movementVector = glm::vec3(0.0f, 0.0f, 0.0f);

        // https://www.youtube.com/watch?v=v3zT3Z5apaM&t=1s

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

        bool found_collision = false;

        // Collision detection
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i].enabled == false)
            {
                continue;
            }

            const auto one = cameraPos;
            const auto two = objects[i].transform;
            // Expanding the scale to stop camera phasing through the wall

            float expansion = 0.5f;

            glm::vec3 minBounds =
                {
                    two.pos.x - (two.scale.x + expansion) / 2,
                    two.pos.y - (two.scale.y + expansion) / 2,
                    two.pos.z - (two.scale.z + expansion) / 2};

            glm::vec3 maxBounds =
                {
                    two.pos.x + (two.scale.x + expansion) / 2,
                    two.pos.y + (two.scale.y + expansion) / 2,
                    two.pos.z + (two.scale.z + expansion) / 2};

            // Adjust for centered origin by offsetting the position by half the scale
            bool collisionX = (one.x >= minBounds.x && one.x <= maxBounds.x);
            bool collisionY = (one.y >= minBounds.y && (one.y - height) <= maxBounds.y);
            bool collisionZ = (one.z >= minBounds.z && one.z <= maxBounds.z);

            if (collisionX && collisionY && collisionZ && !ignore_collisions)
            {
                //std::cout << objects[i].name << std::endl;

                // Shortest distance to penetrate against X and Z
                float penetrationX = std::min(maxBounds.x - one.x, one.x - minBounds.x);
                float penetrationY = std::min(maxBounds.y - one.y, one.y - height - minBounds.y);
                float penetrationZ = std::min(maxBounds.z - one.z, one.z - minBounds.z);

                // Closer to the X axis
                if (penetrationX < penetrationZ && penetrationX < penetrationY) // X collision
                {
                    float direction;
                    if (one.x - two.pos.x < 0.0f)
                    {
                        direction = -1.0f;
                    }
                    else
                    {
                        direction = 1.0f;
                    }

                    cameraPos.x = two.pos.x + direction * (two.scale.x + expansion) / 2;
                }
                else if (penetrationZ < penetrationY) // Z collision
                {
                    float direction;
                    if (one.z - two.pos.z < 0.0f)
                    {
                        direction = -1.0f;
                    }
                    else
                    {
                        direction = 1.0f;
                    }

                    cameraPos.z = two.pos.z + direction * (two.scale.z + expansion) / 2;
                }
                else // Y collision
                {
                    float direction;
                    if (one.y - two.pos.y < 0.0f) // Hitting from below
                    {
                        direction = -1.0f;
                        targetMovementVector.y -= 3.0f;
                    }
                    else // Hitting from above
                    {
                        found_collision = true;
                        direction = 1.0f;
                        targetMovementVector.y += 3.0f;
                    }
                }
            }
        }

        if (found_collision)
        {
            grounded = true;
        }
        else
        {
            grounded = false;
        }

        //std::cout << grounded << std::endl;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (grounded == true)
            { // Adjust gravity throughout jump
                // targetMovementVector.y = 150.0f;         // Red (0.0 to 1.0);
                grounded = false;
                gravity = 0.0f;
                jumpCooldown = std::chrono::high_resolution_clock::now();
                // cameraPos.y += 1.0f;
            }
        }

        // Jumping
        auto now2 = std::chrono::high_resolution_clock::now();
        auto elapsed2 = std::chrono::duration_cast<std::chrono::milliseconds>(now2 - jumpCooldown).count();
        if (elapsed2 >= 400.0f)
        {
            gravity = originalgravity;
        }
        else
        {
            // printf("JUMPING!\n");
            cameraPos.y += 15.0f * (1.0 - (elapsed2 / 1000) * 9) * deltaTime;
            gravity = originalgravity * ((elapsed2 / 100.0) * 0.33); // 0.3
            //printf("%f - %ld - %f - %f\n", ((elapsed2 / 100.0) * 3.3), elapsed2, elapsed2 / 100.0, cameraPos.y);
            //printf("%f\n", 0.3f * (1.0 - (elapsed2 / 1000) * 9) * deltaTime);
        }
        // Smooth transition to the target vector
        movementVector = glm::mix(movementVector, targetMovementVector, deltaTime * 5.0f);

        if (glm::length(movementVector) > 0.0f)
        {
            float sway = std::sin(glfwGetTime() * swaySpeed) * swayAmount;
            cameraPos.y += sway;
        }

        cameraPos += movementVector;
    }
    else // Is level editing
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
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

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
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
        mouse_x = xpos;
        mouse_y = ypos;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // direction.y = sin(glm::radians(pitch));
    direction.y = 0.0f;
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    // cameraFront = glm::normalize(direction);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) // Currently only works on the first object not any of the others, likely cause, returning at the end after making an object
{                                                                                // causing both loop (looping through objects and vertices) to exit
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !debounce) // https://antongerdelan.net/opengl/raycasting.html
    {
        float closest_intersection = std::numeric_limits<float>::max(); // Largest possible number (Technical limit)
        bool found_intersection = false;
        object intersection_object;

        for (int i = 0; i < objects.size(); i++) // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
        {
            for (int v = 0; v < objects[i].vertices.size(); v += 24) // 8 is the vertex size but we need a vertice so 3 vertexes (8*3)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, objects[i].transform.pos);
                model = glm::scale(model, glm::vec3(objects[i].transform.scale));

                glm::vec3 V0 = glm::vec3(model * glm::vec4(objects[i].vertices[v], objects[i].vertices[v+1], objects[i].vertices[v+2], 1.0f)); // Vertex 1
                glm::vec3 V1 = glm::vec3(model * glm::vec4(objects[i].vertices[v+8], objects[i].vertices[v+9], objects[i].vertices[v+10], 1.0f)); // Vertex 2
                glm::vec3 V2 = glm::vec3(model * glm::vec4(objects[i].vertices[v+16], objects[i].vertices[v+17], objects[i].vertices[v+18], 1.0f)); // Vertex 3

                glm::vec3 edge1 = V1 - V0; // e1
                glm::vec3 edge2 = V2 - V0; // e2

                glm::vec3 cross_product = glm::cross(cameraFront, edge2); // h

                float dot_product = glm::dot(edge1, cross_product); // a

                if (dot_product > -EPSILON && dot_product < EPSILON)
                {
                    //std::cout << "1" << std::endl;
                    continue;
                }

                float intersection_param = 1.0f / dot_product; // f

                glm::vec3 vector_s = cameraPos - V0; // s

                float dot_product2 = intersection_param * glm::dot(vector_s, cross_product); // u
                
                if (dot_product2 < 0 || dot_product2 > 1) // Outside of triangle
                {
                    //std::cout << "2" << std::endl;
                    continue;
                }

                glm::vec3 cross_product2 = glm::cross(vector_s, edge1); // q

                float dot_product3 = intersection_param * glm::dot(cameraFront, cross_product2); // v

                if (dot_product3 < 0.0f || dot_product2 + dot_product3 > 1.0f)
                {
                    //std::cout << "3" << std::endl;
                    continue;
                }

                float intersection_point = intersection_param * glm::dot(edge2, cross_product2); // t

                if (intersection_point > EPSILON && intersection_point < closest_intersection)
                {
                    closest_intersection = intersection_point;
                    found_intersection = true;
                    intersection_object = objects[i];
                    
                }
            }
        }

        if (found_intersection)
        {
            printf("found intersection");
            glm::vec3 intersectionPos = cameraPos + closest_intersection * cameraFront;

            std::cout << intersectionPos.x << " - " << intersectionPos.y << " - " << intersectionPos.z << std::endl;
            std::cout << "Objects size: " << objects.size() << std::endl;

            add_object(currentIDNumber, "test", cubeVert, true);
            objects.back().transform.pos = intersectionPos;
        }
        else
        {
            printf("no intersection");
        }
    }
}