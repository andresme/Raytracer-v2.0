#include "objects.h"
#include <stdlib.h>

void createObject(objectNode *newObject, rgb newColor, int type, long double color[], long double amb, long double ks, int kn, long double o1, long double o2){
    newColor.r = color[0];
    newColor.g = color[1];
    newColor.b = color[2];

    newObject->type = type;
    newObject->color = newColor;
    newObject->kn = kn;
    newObject->ambient = amb;
    newObject->ks = ks;
    newObject->o1 = o1;
    newObject->o2 = o2;
}

objectNode* addObject(objectNode *newObject, objectNode *objects){
    if(!objects){
        objects = newObject;
        objects->next = NULL;
    }
    else{
        objectNode *current = objects;
        while(current->next){
            current = current->next;
        }
        current->next = newObject;
        current->next->next = NULL;

    }
    return objects;
}