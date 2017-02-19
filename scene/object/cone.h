//
// Created by andres on 19/02/17.
//

#ifndef RT2_CONE_H
#define RT2_CONE_H

#include "../../util/struct/sharedStructs.h"
#include "objects.h"

typedef struct cone{
    vector* center;
    vector* Q;
    long double start, end;
    long double u, v;
    long double prop;
} cone;

objectNode * addConeO(int type, long double center[], long double vectorQ[], long double start[], long double p[], long double color[],
                      long double amb, long double ks, int kn, long double o1, long double o2, objectNode *objects);

cone* createCone(long double center[], long double vectorQ[], long double start[], long double p[]);
void freeCone(struct object *this);
vector getNormalCone(vector eye, vector dir, long double t, objectNode *this);
intersectionNode getIntersectionCone(vector dir, vector anchor, objectNode *object);

#endif //RT2_CONE_H
