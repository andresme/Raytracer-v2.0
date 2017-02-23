//
// Created by andres on 17/02/17.
//

#ifndef RT2_LIGHT_H
#define RT2_LIGHT_H

#include "../../util/struct/sharedStructs.h"

typedef struct light {
    vector center;
    long double intensity;
    long double c[3];
    rgb color;
    struct light *next;
} lightNode;

lightNode * addLight(long double center[], long double intensity, long double c[], long double color[], lightNode *lights);

#endif //RT2_LIGHT_H
