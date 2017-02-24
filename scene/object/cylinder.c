#include <stdlib.h>
#include <math.h>
#include "cylinder.h"
#include "../../util/math/mathUtils.h"

objectNode* addCylinderO(int type, long double center[], long double N[],long double G[], long double start[], long double r,
                         long double color[], long double amb, long double ks, int kn, long double o1, long double o2, int style, objectNode *objects){
    objectNode *newObject = (objectNode *) malloc(sizeof(struct object));
    rgb newColor = {0, 0, 0};
    cylinder *newCylinder;

    createObject(newObject, newColor, type, color, amb, ks, kn, o1, o2);
    newCylinder = createCylinder(center, N, start, r, G);
    newObject->info = (void *) newCylinder;
    newObject->texture = style;
    newObject->getNormal = &getNormalCylinder;
    newObject->getIntersection = &getIntersectionCylinder;
    newObject->deleteObject = &freeCylinder;
    newObject->getTexture = &getTextureCylinder;

    return addObject(newObject, objects);
}

cylinder* createCylinder(long double center[], long double vectorQ[], long double start[], long double r, long double vectorG[]){
    cylinder *newCylinder = (cylinder *)malloc(sizeof(cylinder));
    vector newCenter = {0, 0, 0};
    vector newVectorQ = {0, 0, 0};
    vector newVectorG = {0, 0, 0};

    newCenter.x = center[0];
    newCenter.y = center[1];
    newCenter.z = center[2];
    newCylinder->center = newCenter;

    newVectorQ.x = vectorQ[0];
    newVectorQ.y = vectorQ[1];
    newVectorQ.z = vectorQ[2];
    newCylinder->Q = newVectorQ;

    newVectorG.x = vectorG[0];
    newVectorG.y = vectorG[1];
    newVectorG.z = vectorG[2];
    newCylinder->G = newVectorG;

    newCylinder->start = start[0];
    newCylinder->end = start[1];
    newCylinder->r2 = r*r;
    newCylinder->r = r;

    return newCylinder;
}

void freeCylinder(struct object *this){
    cylinder *obj = (cylinder *) this->info;
    free(obj);
}

vector getNormalCylinder(vector eye, vector dir, long double t, objectNode *this){
    cylinder *obj = (cylinder *) this->info;
    vector N = {0, 0, 0};
    long double xi, yi, zi, x0, y0, z0, xq, yq, zq, l, d;

    xi = eye.x + (t * dir.x);
    yi = eye.y + (t * dir.y);
    zi = eye.z + (t * dir.z);

    x0 = obj->center.x;
    y0 = obj->center.y;
    z0 = obj->center.z;

    xq = obj->Q.x;
    yq = obj->Q.y;
    zq = obj->Q.z;

    d = ((xi-x0)*xq)+((yi-y0)*yq)+((zi-z0)*zq);

    l = obj->r;
    N.x = (xi-(x0+(d*xq)))/l;
    N.y = (yi-(y0+(d*yq)))/l;
    N.z = (zi-(z0+(d*zq)))/l;


    return N;
}

intersectionNode getIntersectionCylinder(vector dir, vector anchor, objectNode *object){
    long double Xc,Yc,Zc, Xa,Ya,Za, Xd, Yd, Zd, r, Xq, Yq, Zq;
    long double a,b,c,d,e,f, Vx,Vy,Vz;
    intersectionNode intersec = {0, NULL, NULL};
    cylinder *this = (cylinder *) object->info;
    long double B, Y, D, A;
    long double t1, t2;

    Xc = this->center.x;
    Yc = this->center.y;
    Zc = this->center.z;

    r = this->r2;

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

    A = pow2(a)+pow2(b)+pow2(c);

    B = (2*a*d*(pow2(Xq)-1))+(2*a*Xq*((Yq*e)+(Zq*f)))
        +(2*b*e*(pow2(Yq)-1))+(2*b*Yq*((Xq*d)+(Zq*f)))
        +(2*c*f*(pow2(Zq)-1))+(2*c*Zq*((Xq*d)+(Yq*e)));

    Y = pow2(((pow2(Xq)-1)*d))+pow2((Xq*((Yq*e)+(Zq*f))))
        +pow2(((pow2(Yq)-1)*e))+pow2((Yq*((Xq*d)+(Zq*f))))
        +pow2(((pow2(Zq)-1)*f))+pow2((Zq*((Xq*d)+(Yq*e))))
        +(2*d*Xq*(pow2(Xq)-1)*((Yq*e)+(Zq*f)))
        +(2*e*Yq*(pow2(Yq)-1)*((Xq*d)+(Zq*f)))
        +(2*f*Zq*(pow2(Zq)-1)*((Xq*d)+(Yq*e)))-r;

    //Calcula discriminante
    D = pow2(B)-(4*A*Y);

    if(D < DELTA){
        return intersec;
    }
        //Determina si devolver t1 o t2
    else if(D > 0){
        t1 = ((-1 * B) + sqrt(D))/(2 * A);
        t2 = ((-1 * B) - sqrt(D))/(2 * A);
        if(t1 < t2 && t1 > DELTA){

            Vx = Xa + t1*Xd;
            Vy = Ya + t1*Yd;
            Vz = Za + t1*Zd;
            d = (Vx-this->center.x)*this->Q.x +
                (Vy-this->center.y)*this->Q.y +
                (Vz-this->center.z)*this->Q.z;
            if(this->end == -1.0f || (d < this->end && d > this->start)){
                intersec.object = object;
                intersec.t = t1;
            }
            else{
                Vx = Xa + t2*Xd;
                Vy = Ya + t2*Yd;
                Vz = Za + t2*Zd;
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
            Vx = Xa + t2*Xd;
            Vy = Ya + t2*Yd;
            Vz = Za + t2*Zd;
            d = (Vx-this->center.x)*this->Q.x +
                (Vy-this->center.y)*this->Q.y +
                (Vz-this->center.z)*this->Q.z;
            if(this->end == -1.0f || (d < this->end && d > this->start)){
                intersec.object = object;
                intersec.t = t2;
            }
            else{
                Vx = Xa + t1*Xd;
                Vy = Ya + t1*Yd;
                Vz = Za + t1*Zd;
                d = (Vx-this->center.x)*this->Q.x +
                    (Vy-this->center.y)*this->Q.y +
                    (Vz-this->center.z)*this->Q.z;
                if(this->end == -1.0f || (d < this->end && d > this->start)){
                    intersec.object = object;
                    intersec.t = t1;
                }
            }
        }

    }
    return intersec;
}

rgb getTextureCylinder(int style, long double t, vector dir, vector anchor, objectNode *object) {
    rgb texel = {0, 0, 0};
    vector N;
    long double v, u, d, Vx, Vy, Vz;
    cylinder *this = (cylinder *) object->info;
    N = getNormalCylinder(anchor,dir,t,object);
    vector G = {this->G.x, this->G.y, this->G.z};
    u = productoPunto(N,G);
    u = acos(u);

    u = u/(2*M_PI);
    Vx = anchor.x + t*dir.x;
    Vy = anchor.y + t*dir.y;
    Vz = anchor.z + t*dir.z;

    d = (Vx-this->center.x)*this->Q.x + (Vy-this->center.y)*this->Q.y +(Vz-this->center.z)*this->Q.z;
    v = (d - this->start)/(this->end-this->start);

    return texel;
}