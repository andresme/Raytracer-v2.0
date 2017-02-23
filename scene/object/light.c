#include <stdlib.h>
#include "light.h"
#include "objects.h"


lightNode* addLight(long double center[], long double intensity, long double c[], long double color[], lightNode *lights){
    lightNode *newLight = malloc(sizeof(struct light));
    vector centerS;
    rgb newColor = {0, 0, 0};

    centerS.x = center[0];
    centerS.y = center[1];
    centerS.z = center[2];

    newColor.r = color[0];
    newColor.g = color[1];
    newColor.b = color[2];

    newLight->center = centerS;

    newLight->color = newColor;

    newLight->intensity = intensity;

    newLight->c[0] = c[0];
    newLight->c[1] = c[1];
    newLight->c[2] = c[2];

    if(!lights){
        lights = newLight;
        lights->next = NULL;
    }
    else{
        lightNode *current = lights;
        while(current->next){
            current = current->next;
        }
        current->next = newLight;
        current->next->next = NULL;
    }
    return lights;
}