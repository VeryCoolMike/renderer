#ifndef WEAPON_MANAGER_H
#define WEAPON_MANAGER_H

#include <stdio.h>

#include "helper.h"
#include "structs.h"

std::chrono::time_point<std::chrono::high_resolution_clock> gunShootTime;

// Extern variables

extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 direction;

extern player playerInstance;

extern int currentIDNumber;

extern float yaw;
extern float pitch;

extern float ambientintensity;


// Manage weapons on click

void fireWeapon(bool debug)
{
    weapon currentWeapon = weapons[playerInstance.weaponID];

    if (currentWeapon.shotgun == true) // Is shotgun
    {
        srand(time(0));

        for (int i = 0; i < currentWeapon.pellets; i++) // Incase of shotgun shot amount of raycasts as the amount of bullets
        {
            
            // this is so PEAK!!!!!
            float offsetX = ((rand() % (int)(((currentWeapon.spread*200.0f)+1.0f) - (currentWeapon.spread*100.0f)) / 100.0f)) - currentWeapon.spread / 2.0f;
            float offsetY = ((rand() % (int)(((currentWeapon.spread*200.0f)+1.0f) - (currentWeapon.spread*100.0f)) / 100.0f)) - currentWeapon.spread / 2.0f;

            std::cout << offsetX << " - " << offsetY << std::endl;

            // Change yaw and pitch and then recalculate instead of this
            glm::vec3 directionTemp;
            directionTemp.x = cos(glm::radians(yaw+offsetX)) * cos(glm::radians(pitch+offsetY));
            directionTemp.y = sin(glm::radians(pitch+offsetY));
            directionTemp.z = sin(glm::radians(yaw+offsetX)) * cos(glm::radians(pitch+offsetY));
            glm::vec3 cameraFrontTemp = glm::normalize(directionTemp);

            ray_cast gunRay = raycast(cameraPos, cameraFrontTemp);

            if (gunRay.valid == true)
            {
                std::cout << "Valid! Hit: " << gunRay.hit.name << std::endl;
                if (debug == true)
                {
                    float random0 = (rand() % 101) / 100.0f;
                    float random1 = (rand() % 101) / 100.0f;
                    float random2 = (rand() % 101) / 100.0f;
                    add_object(currentIDNumber, "test", cubeObj, true);
                    objects.back().transform.scale = glm::vec3(0.1f, 0.1f, 0.1f);
                    objects.back().transform.pos = gunRay.pos;
                    objects.back().objectColor = glm::vec3(random0, random1, random2);

                    lightArray.back().strength = 0.1f;
                }
            }
            else
            {
                std::cout << "Invalid!" << std::endl;
            }
        }
    }
    else // Not shotgun, single fire
    {
        ray_cast gunRay = raycast(cameraPos, cameraFront);

        if (gunRay.valid == true)
        {
            std::cout << "Valid! Hit: " << gunRay.hit.name << std::endl;
        }
        else
        {
            std::cout << "Invalid!" << std::endl;
        }
    }

    
}

#endif // WEAPON_MANAGER_H