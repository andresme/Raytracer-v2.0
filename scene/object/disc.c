#include <stdlib.h>
#include <math.h>
#include "disc.h"
#include "../../util/math/mathUtils.h"


objectNode* addDiscO(int type, long double center[], long double N[], long double r, long double r2, long double color[],
                     long double amb, long double ks, int kn, long double o1, long double o2, objectNode *objects){

    objectNode *newObject = malloc(sizeof *newObject);
    rgb newColor = {0, 0, 0};
    disc *newDisc;

    createObject(newObject, newColor, type, color, amb, ks, kn, o1, o2);
    newDisc = createDisc(center, N, r, r2);
    newObject->info = (void *) newDisc;

    newObject->getNormal = &getNormalDisc;
    newObject->getIntersection = &getIntersectionDisc;
    newObject->deleteObject = &freeDisc;
    newObject->texture = 0;
    return addObject(newObject, objects);
}

disc* createDisc(long double center[], long double N[], long double r, long double r2){
    disc *newDisc = malloc(sizeof *newDisc);
    vector newCenter = {0, 0, 0};
    vector newNormal = {0, 0, 0};
    long double temp;

    newCenter.x = center[0];
    newCenter.y = center[1];
    newCenter.z = center[2];
    newDisc->center = newCenter;
    newDisc->A = N[0];
    newDisc->B = N[1];
    newDisc->C = N[2];
    newDisc->D = -(center[0] * N[0] + center[1] * N[1] + center[2] * N[2]);

    temp = sqrt((double) (pow2(newDisc->A) + pow2(newDisc->B) + pow2(newDisc->C)));

    newNormal.x = N[0]/temp;
    newNormal.y = N[1]/temp;
    newNormal.z = N[2]/temp;
    newDisc->N = newNormal;

    newDisc->r = r * r;
    newDisc->r2 = r2 * r2;

    return newDisc;
}

void freeDisc(struct object *this){
    disc* object = (disc *) this->info;
    free(object);
}

vector getNormalDisc(vector eye, vector dir, long double t, objectNode *this){
    disc *obj = (disc *) this->info;
    vector N = {0, 0, 0};
    long double a,b,c;

    a = obj->N.x;
    b = obj->N.y;
    c = obj->N.z;

    N.x = a;
    N.y = b;
    N.z = c;
    return N;
}

intersectionNode getIntersectionDisc(vector dir, vector anchor, objectNode *object){
    intersectionNode intersec = {0, NULL, NULL};
    disc *this = (disc *) object->info;
    long double Xd, Yd, Zd, t, Xi, Yi, Zi;
    long double A, B, C, D, E, F, temp;

    A = this->A;
    B = this->B;
    C = this->C;
    D = this->D;

    Xd = dir.x;
    Yd = dir.y;
    Zd = dir.z;

    F = A*Xd+B*Yd+C*Zd;

    if(F < DELTA && F > -DELTA)
        return intersec;

    E = A*anchor.x+B*anchor.y+C*anchor.z+D;
    t = -E/F;

    if(t < DELTA) return intersec;

    Xi = anchor.x + (t * dir.x);
    Yi = anchor.y + (t * dir.y);
    Zi = anchor.z + (t * dir.z);

    temp = pow2((this->center.x)-Xi) + pow2((this->center.y)-Yi) + pow2((this->center.z)-Zi);

    if(this->r > temp && this->r2 < temp){
        intersec.object = object;
        intersec.t = t;
    }

    return intersec;
}