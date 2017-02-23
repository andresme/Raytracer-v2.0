#ifndef RT2_DISC_H
#define RT2_DISC_H

#include "../../util/struct/sharedStructs.h"
#include "objects.h"

typedef struct disc{
    vector *center;
    vector *N;
    long double r, r2, A, B, C, D;
} disc;

objectNode* addDiscO(int type, long double center[], long double N[], long double r, long double r2, long double color[],
                     long double amb, long double ks, int kn, long double o1, long double o2, objectNode *objects);

disc* createDisc(long double center[], long double N[], long double r, long double r2);
void freeDisc(struct object *this);
vector getNormalDisc(vector eye, vector dir, long double t, objectNode *this);
intersectionNode getIntersectionDisc(vector dir, vector anchor, objectNode *object);


#endif //RT2_DISC_H
