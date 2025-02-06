#ifndef GUI_H
#define GUI_H

#include <GLFW/glfw3.h>

#include "shader.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Extern variables
extern bool gui_visible;

extern int fileCount;

extern vertices coneObj;
extern vertices dbShotgun;
extern vertices skull;
extern vertices cubeObj;

extern std::vector<GLuint> textureArray;
extern GLfloat backgroundColor[4];
extern float ambientintensity;

extern float lastFPS;
extern float real_camera_speed;
extern glm::vec3 cameraPos;
extern float fov;
extern bool firstMouse;

extern int currentShader;

extern int shadowTexture;

void createGui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    io.FontGlobalScale = 2.0f;
    io.IniFilename = nullptr;

    // Restore main window context
    glfwMakeContextCurrent(window);
}

void renderGui(GLFWwindow *window, Shader regularShader)
{
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

        ImGui::SliderFloat("FOV", &fov, 1.0f, 120.0f);
        ImGui::Text("Camera Position: %.1f, %.1f, %.1f",
                    cameraPos.x, cameraPos.y, cameraPos.z);

        ImGui::Text("FPS: %f", lastFPS);

        static char mapFileName[128] = "MapFile1.txt";

        ImGui::InputText("Map file", mapFileName, sizeof(mapFileName));

        if (ImGui::Button("Save"))
        {
            SaveToFile(mapFileName); // Save to MapFile1.txt
        }

        if (ImGui::Button("Load"))
        {
            guisVisible.clear();
            objects.clear();
            regularShader.setInt("lightAmount", LoadFromFile(mapFileName)); // Load from MapFile1.txt

            for (int n = 0; n < objects.size(); ++n)
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
        }

        if (ImGui::Button("Make Cube")) // Borked, weird interaction when lights exist and spawning regular cubes, but not more lights??? Who knows, Spawning another light fixes???? EDIT: FIXED!!!
        {
            add_object(currentIDNumber, "box", cubeObj, false);
            objects[currentIDNumber - 1].texture = 0;
        }

        if (ImGui::Button("Make Light"))
        {
            add_object(currentIDNumber, "light", cubeObj, true);
            
        }

        ImGui::InputFloat("Camera Speed", &real_camera_speed, 0.1f);

        ImGui::InputFloat("Ambient Strength", &ambientintensity, 0.05f);
        ImGui::ColorPicker4("Background Color", backgroundColor);
        ImGui::InputInt("Current Shader", &currentShader);

        ImGui::Text("DEBUG");
        ImGui::InputInt("Current shadow texture", &shadowTexture);

        ImGui::NewLine();
        ImGui::Text("Browser");
        ImGui::NewLine();

        for (int n = 0; n < objects.size(); ++n)
        {
            if (objects[n].enabled == false)
            {
                continue;
            }
            ImGui::PushID(n); // Push the index as the unique ID
            std::string text = objects[n].name + std::to_string(n);

            if (ImGui::Button(text.c_str()))
            {
                {
                    for (int v = 0; v < guisVisible.size(); v++)
                    {
                        if (guisVisible[v].id == n)
                        {
                            if (guisVisible[v].visible == true)
                            {
                                guisVisible[v].visible = false;
                            }
                            else
                            {
                                guisVisible[v].visible = true;
                            }
                        }
                        else
                        {
                            guisVisible[v].visible = false;
                        }
                    }
                }
            }

            ImGui::PopID(); // Pop the ID after the widget

            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i].selected == true)
                {
                    std::cout << "selected object " << i << std::endl;
                    for (int v = 0; v < guisVisible.size(); v++)
                    {
                        if (guisVisible[v].id == i)
                        {
                            guisVisible[v].visible = true;
                        }
                        else
                        {
                            guisVisible[v].visible = false;
                        }
                    }
                }
            }

        }

        for (int i = 0; i < guisVisible.size(); i++)
        {
            if (guisVisible[i].visible)
            {
                ImGui::Begin(objects[guisVisible[i].id].name.c_str()); // Start the ImGui window
                for (int i = 0; i < fileCount; i++)
                {
                    ImGui::PushID(i);
                    if (ImGui::ImageButton("##texture1", (ImTextureID)(uint64_t)textureArray[i], ImVec2(32, 32), ImVec2(0, 0))) // 0 for no padding
                    {
                        objects[guisVisible[i].id].texture = i;
                    }
                    if (i % 10 != 0 || i == 0)
                    {
                        ImGui::SameLine();
                    }
                    else
                    {
                        ImGui::NewLine();
                    }

                    ImGui::PopID();
                }
                ImGui::NewLine();
                ImGui::SetNextItemWidth(150.0f);
                ImGui::Checkbox("Visible", &objects[guisVisible[i].id].visible);
                ImGui::ColorPicker3("Object Color", glm::value_ptr(objects[guisVisible[i].id].objectColor));
                ImGui::InputFloat3("Position", &objects[guisVisible[i].id].transform.pos.x);
                ImGui::InputFloat3("Rotation", &objects[guisVisible[i].id].transform.rot.x);
                ImGui::InputFloat3("Scale", &objects[guisVisible[i].id].transform.scale.x);
                ImGui::InputFloat("Reflectance", &objects[guisVisible[i].id].reflectance);

                if (objects[guisVisible[i].id].light == true)
                {
                    for (int j = 0; j < lightArray.size(); j++)
                    {
                        if (lightArray[j].id == objects[guisVisible[i].id].id)
                        {
                            ImGui::InputFloat("Light Strength", &lightArray[j].strength);
                        }
                    }
                    
                }
                if (ImGui::Button("Delete"))
                {
                    // Remove the object at index n
                    remove_object(guisVisible[i].id);
                }

                if (ImGui::Button("cube"))
                {
                    updateVertices(cubeObj, objects[guisVisible[i].id]);
                }
                if (ImGui::Button("skull"))
                {
                    updateVertices(skull, objects[guisVisible[i].id]);
                }
                if (ImGui::Button("gun"))
                {
                    updateVertices(dbShotgun, objects[guisVisible[i].id]);
                }
                ImGui::End(); // End the ImGui window
            }
        }
        

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    else // Gui not visible
    {
        for (int i = 0; i < guisVisible.size(); i++)
        {
            guisVisible[i].visible = false;
        }
        for (int v = 0; v < objects.size(); v++)
        {
            objects[v].selected = false;
        }
    }
}

#endif // GUI_H