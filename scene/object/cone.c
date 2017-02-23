//
// Created by andres on 19/02/17.
//

#include <stdlib.h>
#include <math.h>
#include "cone.h"
#include "../../util/math/mathUtils.h"


cone* createCone(long double center[], long double vectorQ[], long double start[], long double p[]){
    cone *newCone = (cone *)malloc(sizeof(cone));
    vector newCenter = {0, 0, 0};
    vector newVectorQ = {0, 0, 0};

    newCenter.x = center[0];
    newCenter.y = center[1];
    newCenter.z = center[2];
    newCone->center = newCenter;

    newVectorQ.x = vectorQ[0];
    newVectorQ.y = vectorQ[1];
    newVectorQ.z = vectorQ[2];
    newCone->Q = newVectorQ;

    newCone->start = start[0];
    newCone->end = start[1];
    newCone->u = p[0];
    newCone->v = p[1];
    newCone->prop = pow2(p[0]/p[1]);

    return newCone;

}

//Calcula la normal del cono
vector getNormalCone(vector eye, vector dir, long double t, objectNode *this){

    cone *obj = (cone *) this->info;
    vector N = {0, 0, 0};
    long double xi, yi, zi, x0, y0, z0, xq, yq, zq, xl, yl, zl, l, cosLQ, H;
    xi = eye.x + (t * dir.x);
    yi = eye.y + (t * dir.y);
    zi = eye.z + (t * dir.z);

    x0 = obj->center.x;
    y0 = obj->center.y;
    z0 = obj->center.z;

    xq = obj->Q.x;
    yq = obj->Q.y;
    zq = obj->Q.z;

    xl = xi-x0;
    yl = yi-y0;
    zl = zi-z0;

    l = sqrt((double) (pow2(xl) + pow2(yl) + pow2(zl)));

    xl = xl/l;
    yl = yl/l;
    zl = zl/l;

    cosLQ = (xq*xl+yq*yl+zq*zl);
    H = l/cosLQ;

    N.x = xi-(x0+H*xq);
    N.y = yi-(y0+H*yq);
    N.z = zi-(z0+H*zq);

    l = sqrt((double) (pow2(N.x) + pow2(N.y) + pow2(N.z)));

    N.x = N.x/l;
    N.y = N.y/l;
    N.z = N.z/l;

    return N;
}

//Calcula las intersecciones con el cono
intersectionNode getIntersectionCone(vector dir, vector anchor, objectNode *object){
    long double Xc,Yc,Zc, Xa,Ya,Za, Xd, Yd, Zd, Xq, Yq, Zq;
    long double a,b,c,d,e,f,g,h, Vx,Vy,Vz;
    intersectionNode intersec = {0, NULL, NULL};
    cone *this = (cone *) object->info;
    long double B, Y, D, A;
    long double t1, t2;

    Xc = this->center.x;
    Yc = this->center.y;
    Zc = this->center.z;

    Xq = this->Q.x;
    Yq = this->Q.y;
    Zq = this->Q.z;

    Xa = anchor.x;
    Ya = anchor.y;
    Za = anchor.z;

    Xd = dir.x;
    Yd = dir.y;
    Zd = dir.z;

    a = (pow2(Xq)*Xd)+(Xq*Yd*Yq)+(Xq*Zd*Zq)-Xd;
    b = (pow2(Yq)*Yd)+(Xd*Xq*Yq)+(Yq*Zd*Zq)-Yd;
    c = (pow2(Zq)*Zd)+(Xd*Xq*Zq)+(Yd*Yq*Zq)-Zd;
    d = (Xa-Xc);
    e = (Ya-Yc);
    f = (Za-Zc);
    g = (Xd*Xq)+(Yd*Yq)+(Zd*Zq);
    h = this->prop;

    A = pow2(a)+pow2(b)+pow2(c)-(h*pow2(g));

    B = (2*a*d*(pow2(Xq)-1))+(2*a*Xq*((Yq*e)+(Zq*f)))
        +(2*b*e*(pow2(Yq)-1))+(2*b*Yq*((Xq*d)+(Zq*f)))
        +(2*c*f*(pow2(Zq)-1))+(2*c*Zq*((Xq*d)+(Yq*e)))
        -(h*2*g*d*Xq)-(h*2*g*e*Yq)-(h*2*g*f*Zq);

    Y = pow2((pow2(Xq)-1)*d)+pow2(Xq*((Yq*e)+(Zq*f)))
        +pow2((pow2(Yq)-1)*e)+pow2(Yq*((Xq*d)+(Zq*f)))
        +pow2((pow2(Zq)-1)*f)+pow2(Zq*((Xq*d)+(Yq*e)))
        +(2*d*Xq*(pow2(Xq)-1)*((Yq*e)+(Zq*f)))
        +(2*e*Yq*(pow2(Yq)-1)*((Xq*d)+(Zq*f)))
        +(2*f*Zq*(pow2(Zq)-1)*((Xq*d)+(Yq*e)))
        -(h*pow2(Xq*d))-(h*pow2(Yq*e))-(h*pow2(Zq*f))
        -(h*2*d*e*Xq*Yq)-(h*2*d*f*Xq*Zq)-(h*2*e*f*Yq*Zq);

    D = pow2(B) - (4*A*Y);

    if(D < DELTA){
        return intersec;
    }
        //Determina si devolver t1 o t2
    else if(D > 0){
        t1 = ((-1 * B) + sqrt(D))/(2 * A);
        t2 = ((-1 * B) - sqrt(D))/(2 * A);
        if(t1 < t2 && t1 > DELTA){

            Vx =  Xa + t1*Xd;
            Vy =  Ya + t1*Yd;
            Vz =  Za + t1*Zd;
            d = (Vx-this->center.x)*this->Q.x +
                (Vy-this->center.y)*this->Q.y +
                (Vz-this->center.z)*this->Q.z;
            if(d < this->end && d > this->start){
                intersec.object = object;
                intersec.t = t1;
            }
            else{
                Vx =  Xa + t2*Xd;
                Vy =  Ya + t2*Yd;
                Vz =  Za + t2*Zd;
                d = (Vx-this->center.x)*this->Q.x +
                    (Vy-this->center.y)*this->Q.y +
                    (Vz-this->center.z)*this->Q.z;
                if(d < this->end && d > this->start){
                    intersec.object = object;
                    intersec.t = t2;
                }
            }
        }
        else if(t2 < t1 && t2 > DELTA){

            Vx =  Xa + t2*Xd;
            Vy =  Ya + t2*Yd;
            Vz =  Za + t2*Zd;
            d = (Vx-this->center.x)*this->Q.x +
                (Vy-this->center.y)*this->Q.y +
                (Vz-this->center.z)*this->Q.z;
            if(d < this->end && d > this->start){
                intersec.object = object;
                intersec.t = t2;
            }
            else{
                Vx =  Xa + t1*Xd;
                Vy =  Ya + t1*Yd;
                Vz =  Za + t1*Zd;
                d = (Vx-this->center.x)*this->Q.x +
                    (Vy-this->center.y)*this->Q.y +
                    (Vz-this->center.z)*this->Q.z;
                if(d < this->end && d > this->start){
                    intersec.object = object;
                    intersec.t = t1;
                }
            }
        }
    }
    return intersec;
}

void freeCone(struct object *this) {
    cone *thisCone = (cone *) this;
    free(thisCone);
}

objectNode* addConeO(int type, long double center[], long double vectorQ[], long double start[], long double p[], long double color[],
              long double amb, long double ks, int kn, long double o1, long double o2, objectNode *objects){
    objectNode *newObject = (objectNode *) malloc(sizeof(struct object));
    rgb newColor = {0, 0, 0};
    cone *newCone;

    createObject(newObject, newColor, type, color, amb, ks, kn, o1, o2);
    newCone = createCone(center, vectorQ, start, p);
    newObject->info = (void *) newCone;

    newObject->getNormal = &getNormalCone;
    newObject->getIntersection = &getIntersectionCone;
    newObject->deleteObject = &freeCone;

    return addObject(newObject, objects);

}