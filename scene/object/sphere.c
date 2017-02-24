#include <stdlib.h>
#include "sphere.h"
#include "polygon.h"

sphere* createSphere(long double center[], long double radius, int cantCortes, plane* cortes) {
    sphere *newSphere = (sphere *)malloc(sizeof(sphere));
    vector centerS = {0, 0, 0};

    centerS.y = center[0];
    centerS.x = center[1];
    centerS.z = center[2];

    newSphere->center = centerS;
    newSphere->radius = radius;
    newSphere->radius2 = radius * radius;
    newSphere->cantCortes = cantCortes;
    newSphere->cortes = cortes;

    return newSphere;
}

void freeSphere(struct object *this){
    sphere *thisSphere = (sphere *) this->info;
    if(thisSphere->cantCortes > 0){
        free(thisSphere->cortes);
    }
    free(thisSphere);
}

intersectionNode getIntersectionSphere(vector dir, vector anchor, struct object *this){
    long double Xc, Yc, Zc, Xa, Ya, Za, Xd, Yd, Zd, r2;
    intersectionNode intersection = {0, NULL, NULL};
    sphere* object = (sphere *) this->info;

    long double B, Y, D;
    long double t1, t2;

    Xc = object->center.x;
    Yc = object->center.y;
    Zc = object->center.z;

    Xa = anchor.x;
    Ya = anchor.y;
    Za = anchor.z;

    Xd = dir.x;
    Yd = dir.y;
    Zd = dir.z;

    r2 = object->radius2;

    B = 2 * ( (Xa-Xc) * Xd + (Ya-Yc) * Yd + (Za-Zc) * Zd );
    Y = pow2(Xa-Xc) + pow2(Ya-Yc) + pow2(Za-Zc) - r2;
    D = pow2(B) - 4 * Y;

    if(D < DELTA) {
        return intersection;
    } else {
        t1 = (-B + sqrt((double) D)) / 2;
        t2 = (-B - sqrt((double) D)) / 2;
        if(t2 < DELTA && t1 > DELTA) {
            intersection.object = this;
            intersection.t = t1;
        } else if(t1 > DELTA && t2 > DELTA) {
            intersection.object = this;
            intersection.t = t2;
        }
    }
    if(intersection.object != NULL && object->cantCortes > 0) {
        for(int contador = 0; contador < object->cantCortes; contador++){
            long double *t = calcIntersecPlane0(dir,anchor,object->cortes[contador]);
            if(t != NULL){
                if(t2-t[3] < DELTA){
                    intersection.t = t1;
                }
            }
        }
    }
    return intersection;
}

vector getNormalSphere(vector eye, vector dir, long double t, objectNode *this){
    vector normalizedVector = {0, 0, 0};
    sphere *object = (sphere*) this->info;
    long double Xi, Yi, Zi, radius;

    Xi = eye.x + (t * dir.x);
    Yi = eye.y + (t * dir.y);
    Zi = eye.z + (t * dir.z);

    radius = object->radius;

    normalizedVector.x = Xi - object->center.x;
    normalizedVector.y = Yi - object->center.y;
    normalizedVector.z = Zi - object->center.z;

    normalizedVector.x = (normalizedVector.x)/radius;
    normalizedVector.y = (normalizedVector.y)/radius;
    normalizedVector.z = (normalizedVector.z)/radius;
    return normalizedVector;
}

//Agrega una esfera
objectNode* addSphereO(int type, long double center[], long double radius, long double color[], long double amb, long double ks,
                int kn, long double o1, long double o2, int cantCortes, vector* cortes, vector* nCortes, objectNode *objects){
    objectNode *newObject = (objectNode *) malloc(sizeof(struct object));
    plane *planosCorte = NULL;

    rgb newColor = {0, 0, 0};
    sphere *newSphere;

    createObject(newObject, newColor, type, color, amb, ks, kn, o1, o2);

    if(cantCortes > 0){
        planosCorte = calcPlanosDeCorte(cantCortes, cortes, nCortes);
    }


    newSphere = createSphere(center, radius, cantCortes, planosCorte);
    newObject->info = (void *) newSphere;
    newObject->getNormal = &getNormalSphere;
    newObject->getIntersection = &getIntersectionSphere;
    newObject->deleteObject = &freeSphere;

    return addObject(newObject, objects);

}
