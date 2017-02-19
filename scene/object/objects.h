#ifndef OBJECTS_H
#define OBJECTS_H

#include "../../util/struct/sharedStructs.h"

typedef struct intersection {
    long double t;
    struct object *object;
    struct intersection *next;
} intersectionNode;

typedef struct object {
    char *textureFile;
    int type;
    int kn;
    int texture;
    void *info;
    rgb* color;
    long double ambient;
    long double ks;
    long double o1;
    long double o2;
    rgb (*getTexture) (int style, long double t, vector dir, vector anchor, struct object *this);
    vector (*getNormal) (vector eye, vector dir, long double t, struct object *this);
    intersectionNode (*getIntersection)(vector dir, vector anchor, struct object *this);
    void (*deleteObject)(struct object *this);
    struct object *next;
} objectNode;




objectNode * addObject(objectNode *newObject, objectNode *objects);
void createObject(objectNode *newObject, rgb *newColor, int type, long double color[], long double amb, long double ks, int kn, long double o1, long double o2);

#endif
