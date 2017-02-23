//
// Created by andres on 22/02/17.
//

#ifndef RT2_CYLINDER_H
#define RT2_CYLINDER_H

#include "objects.h"

typedef struct cylinder{
    vector* center;
    vector* Q, *G;
    long double start, end;
    long double r;
    long double r2;
} cylinder;

objectNode* addCylinderO(int type, long double center[], long double N[],long double G[], long double start[], long double r,
                         long double color[], long double amb, long double ks, int kn, long double o1, long double o2, int style, objectNode *objects);

cylinder* createCylinder(long double center[], long double vectorQ[], long double start[], long double r, long double vectorG[]);
void freeCylinder(struct object *this);
vector getNormalCylinder(vector eye, vector dir, long double t, objectNode *this);
intersectionNode getIntersectionCylinder(vector dir, vector anchor, objectNode *object);
rgb getTextureCylinder(int style, long double t, vector dir, vector anchor, objectNode *object);

#endif //RT2_CYLINDER_H
