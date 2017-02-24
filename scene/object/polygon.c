#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "polygon.h"
#include "../../util/math/mathUtils.h"
#include "../../util/io/fileUtils.h"

void shapeRectangle(polygon* poli);
vector getMaximos(int cantVertex, vector *vertices);
vector getMinimos(int cantVertex, vector *vertices);

objectNode* addPolygonO(int type, vector* vertices, int cantidad, long double color[], long double amb, long double ks,
                        int kn, long double o1, long double o2, int style, char *imag, objectNode *objects){

    objectNode *newObject = (objectNode *) malloc(sizeof(struct object));
    rgb newColor = {0, 0, 0};
    polygon *newPolygon;

    createObject(newObject, newColor, type, color, amb, ks, kn, o1, o2);
    newPolygon = createPolygon(vertices, cantidad);
    newObject->info = (void *) newPolygon;
    newObject->texture = style;
    newObject->getNormal = &getNormalPolygon;
    newObject->getIntersection = &getIntersectionPolygon;
    newObject->getTexture = &getTexturePolygon;
    newObject->deleteObject = &freePolygon;
    newObject->textureFile = (char *) malloc((strlen(imag)+1)*sizeof(char));
    strcpy(newObject->textureFile, imag);
    newObject->textureFile[strlen(imag)] = '\0';

    return addObject(newObject, objects);
}

polygon* createPolygon(vector* vertices, int cantidad){
    polygon *newPolygon = (polygon *) malloc(sizeof(polygon));

    newPolygon->plano = createPlane(vertices);
    newPolygon->cantVertex = cantidad;
    newPolygon->rectangle = NULL;
    newPolygon->vertices = vertices;
    flatVertex(newPolygon, cantidad, vertices);

    return newPolygon;
}

void freePolygon(struct object *this){
    polygon *obj = (polygon *) this->info;
    free(obj->rectangle);
    free(obj->vertices);
    free(obj->vertices2D);
    free(obj);
}

vector getNormalPolygon(vector eye, vector dir, long double t, objectNode *this){
    polygon *obj = (polygon *) this->info;
    vector N = {0, 0, 0};
    N.x = obj->plano.N.x;
    N.y = obj->plano.N.y;
    N.z = obj->plano.N.z;

    return N;
}

intersectionNode getIntersectionPolygon(vector dir, vector anchor, objectNode *object){
    intersectionNode intersec = {0, NULL, NULL};
    polygon *this = (polygon *) object->info;
    long double *planeInter = calcIntersecPlane(dir, anchor, this->plano);
    if((planeInter) && (pointExists(planeInter, object))){
        intersec.object = object;
        intersec.t = planeInter[3];
    }
    free(planeInter);
    return intersec;
}

rgb getTexturePolygon(int style, long double t, vector dir, vector anchor, objectNode *object){
    rgb texel;
    polygon *this = (polygon *) object->info;
    long double u, v, d, h,  Xi, Yi, Zi;
    vector I = {0, 0, 0};

    if(this->rectangle == NULL)
        shapeRectangle(this);

    Xi = anchor.x + (t * dir.x);
    Yi = anchor.y + (t * dir.y);
    Zi = anchor.z + (t * dir.z);

    I.x = Xi - this->rectangle[0].x;
    I.y = Yi - this->rectangle[0].y;
    I.z = Zi - this->rectangle[0].z;
    d = productoPunto(I,this->U);
    h = productoPunto(I,this->V);

    if(object->texture == 1){
        u = d/this->w;
        v = h/this->h;
    }
    else{
        u = 0;
        v = 0;
    }
    texel = getTexel(u,v,d,h,object);
    return texel;
}


long double* calcIntersecPlane(vector dir, vector eye, plane plano){
    long double *intersec = NULL;
    long double Xd, Yd, Zd, t, Xi, Yi, Zi;
    long double A, B, C, D, E, F;

    A = plano.A;
    B = plano.B;
    C = plano.C;
    D = plano.D;

    Xd = dir.x;
    Yd = dir.y;
    Zd = dir.z;

    F = A*Xd+B*Yd+C*Zd;
    if(F < DELTA && F > -DELTA)
        return intersec;

    E = A*eye.x+B*eye.y+C*eye.z+D;
    t = -E/F;

    if((t > -DELTA)&&(t < DELTA))
        return intersec;

    Xi = eye.x + (t * dir.x);
    Yi = eye.y + (t * dir.y);
    Zi = eye.z + (t * dir.z);

    intersec = (long double*) malloc(4*sizeof(long double));
    intersec[0] = Xi;
    intersec[1] = Yi;
    intersec[2] = Zi;
    intersec[3] = t;
    return intersec;
}

long double* calcIntersecPlane0(vector dir, vector anchor, plane plano){
    long double *intersec = NULL;
    long double Xd, Yd, Zd, t, Xi, Yi, Zi;
    long double A, B, C, D, E, F;

    A = plano.A;
    B = plano.B;
    C = plano.C;
    D = plano.D;

    Xd = dir.x;
    Yd = dir.y;
    Zd = dir.z;

    F = A*Xd+B*Yd+C*Zd;
    if(F < DELTA && F > -DELTA)
        return intersec;

    E = A*anchor.x+B*anchor.y+C*anchor.z+D;
    t = -E/F;

    if((t > -DELTA)&&(t < DELTA))
        return intersec;

    Xi = anchor.x + (t * dir.x);
    Yi = anchor.y + (t * dir.y);
    Zi = anchor.z + (t * dir.z);

    intersec = (long double*) malloc(4*sizeof(long double));
    intersec[0] = Xi;
    intersec[1] = Yi;
    intersec[2] = Zi;
    intersec[3] = t;
    return intersec;
}

void flatVertex(polygon *newPolygon, int cantidad, vector *vertices){
    vertex2D *newVertex = (vertex2D *) malloc(cantidad*sizeof(vertex2D));
    long double a, b, c;
    int i;
    a = absld(newPolygon->plano.N.x);
    b = absld(newPolygon->plano.N.y);
    c = absld(newPolygon->plano.N.z);
    if(a>b && a>c){
        for(i = 0; i < cantidad; i++){
            newVertex[i].U = vertices[i].y;
            newVertex[i].V = vertices[i].z;
            newPolygon->flag = 0;
        }
    }
    else if(b>a && b>c){
        for(i = 0; i < cantidad; i++){
            newVertex[i].U = vertices[i].x;
            newVertex[i].V = vertices[i].z;
            newPolygon->flag = 1;
        }
    }
    else{
        for(i = 0; i < cantidad; i++){
            newVertex[i].U = vertices[i].x;
            newVertex[i].V = vertices[i].y;
            newPolygon->flag = 2;
        }
    }
    newPolygon->vertices2D = newVertex;
}

int pointExists(long double* inter, objectNode *object){
    polygon *this = (polygon *) object->info;
    vertex2D* vertices2D = this->vertices2D;

    int contador, contador_paredes;
    long double b,u,v,x1,y1,x2,y2,m;

    contador_paredes = 0;

    if(this->flag == 0){
        u = inter[1];
        v = inter[2];
    }
    else if(this->flag == 1){
        u = inter[0];
        v = inter[2];
    }
    else{
        u = inter[0];
        v = inter[1];
    }

    for(contador = 0; contador < this->cantVertex-1; contador++){

        x1 = vertices2D[contador].U - u;
        y1 = vertices2D[contador].V - v;
        x2 = vertices2D[contador+1].U - u;
        y2 = vertices2D[contador+1].V - v;

        if((x1 > DELTA) && (x2 > DELTA) && ((y1 < -DELTA && y2 > DELTA)||(y2 < -DELTA && y1 > DELTA)))
            contador_paredes++;
        else if(((x1 < -DELTA && x2 > DELTA)||(x2 < -DELTA && x1 > DELTA))&&((y1 < -DELTA && y2 > DELTA)||(y2 < -DELTA && y1 > DELTA))){
            if(!(x2-x1 > -DELTA && x2-x1 < DELTA)){
                m = (y2-y1)/(x2-x1);
                if(!(m> -DELTA && m < DELTA)){
                    b = y1-(m*x1);
                    long double result = -b/m;
                    if(result > DELTA)
                        contador_paredes++;
                }
            }
        }
    }
    x1 = vertices2D[this->cantVertex-1].U - u;
    y1 = vertices2D[this->cantVertex-1].V - v;
    x2 = vertices2D[0].U - u;
    y2 = vertices2D[0].V - v;

    if((x1 > DELTA) && (x2 > DELTA) && ((y1 < -DELTA && y2 > DELTA)||(y2 < -DELTA && y1 > DELTA)))
        contador_paredes++;
    else if(((x1 < -DELTA && x2 > DELTA)||(x2 < -DELTA && x1 > DELTA))&&((y1 < -DELTA && y2 > DELTA)||(y2 < -DELTA && y1 > DELTA))){
        if(!(x2-x1 > -DELTA && x2-x1 < DELTA)){
            m = (y2-y1)/(x2-x1);
            if(!(m> -DELTA && m < DELTA)){
                b = y1-(m*x1);
                long double result = -b/m;
                if(result > DELTA)
                    contador_paredes++;
            }
        }
    }

    if(contador_paredes%2 == 1){
        return 1;
    }
    else return 0;
}

plane createPlane(vector *vertices){
    plane newPlane = {{0, 0, 0}, 0, 0, 0, 0};

    vector newNormal = {0, 0, 0};
    vector v = {0, 0, 0};
    vector w = {0, 0, 0};

    long double a, b, c, d, L;


    v.x = vertices[1].x - vertices[0].x;
    v.y = vertices[1].y - vertices[0].y;
    v.z = vertices[1].z - vertices[0].z;

    w.x = vertices[2].x - vertices[0].x;
    w.y = vertices[2].y - vertices[0].y;
    w.z = vertices[2].z - vertices[0].z;

    a = (v.y * w.z) - (w.y * v.z);
    b = (w.x * v.z) - (v.x * w.z);
    c = (v.x * w.y) - (w.x * v.y);
    d = -((a*vertices[0].x)+(b*vertices[0].y)+(c*vertices[0].z));


    L= sqrt((double) (pow2(a) + pow2(b) + pow2(c)));

    a = a/L;
    b = b/L;
    c = c/L;
    d = d/L;

    newNormal.x = a;
    newNormal.y = b;
    newNormal.z = c;

    newPlane.N = newNormal;

    newPlane.A = a;
    newPlane.B = b;
    newPlane.C = c;
    newPlane.D = d;

    return newPlane;
}

plane createPlane0(vector vertice, vector normal){
    plane newPlane = {{0, 0, 0}, 0, 0, 0, 0};

    vector newNormal = {0, 0, 0};

    long double a, b, c, d, L;
    a = normal.x;
    b = normal.y;
    c = normal.z;
    d = -((a*vertice.x)+(b*vertice.y)+(c*vertice.z));

    L= sqrt((double) (pow2(a) + pow2(b) + pow2(c)));

    a = a/L;
    b = b/L;
    c = c/L;
    d = d/L;

    newNormal.x = a;
    newNormal.y = b;
    newNormal.z = c;

    newPlane.N = newNormal;

    newPlane.A = a;
    newPlane.B = b;
    newPlane.C = c;
    newPlane.D = d;

    return newPlane;
}


vector getMinimos(int cantVertex, vector *vertices){
    int contador;
    vector mins;

    mins.x = vertices[0].x;
    mins.y = vertices[0].y;
    mins.z = vertices[0].z;

    for(contador = 1; contador < cantVertex; contador++){
        if(vertices[contador].x < mins.x)
            mins.x = vertices[contador].x;
        if(vertices[contador].y < mins.y)
            mins.y = vertices[contador].y;
        if(vertices[contador].z < mins.z)
            mins.z = vertices[contador].z;
    }

    return mins;
}

vector getMaximos(int cantVertex, vector *vertices){
    int contador;
    vector maxs;

    maxs.x = vertices[0].x;
    maxs.y = vertices[0].y;
    maxs.z = vertices[0].z;

    for(contador = 1; contador < cantVertex; contador++){
        if(vertices[contador].x > maxs.x)
            maxs.x = vertices[contador].x;
        if(vertices[contador].y > maxs.y)
            maxs.y = vertices[contador].y;
        if(vertices[contador].z > maxs.z)
            maxs.z = vertices[contador].z;
    }

    return maxs;
}


void shapeRectangle(polygon* poli){
    vector *vertices = (vector*) malloc(4*sizeof(vector));

    vertices[0] = getMinimos(poli->cantVertex, poli->vertices);
    vertices[2] = getMaximos(poli->cantVertex, poli->vertices);

    vertices[1].x = vertices[0].x;
    vertices[1].y = vertices[2].y;
    vertices[1].z = vertices[2].z;

    vertices[3].x = vertices[2].x;
    vertices[3].y = vertices[0].y;
    vertices[3].z = vertices[0].z;

    poli->w = (long double) sqrt(
            (double) (pow2((vertices[1].x - vertices[0].x)) + pow2((vertices[1].y - vertices[0].y)) + pow2((vertices[1].z - vertices[0].z))));

    poli->U.x = (vertices[1].x-vertices[0].x)/poli->w;
    poli->U.y = (vertices[1].y-vertices[0].y)/poli->w;
    poli->U.z = (vertices[1].z-vertices[0].z)/poli->w;

    poli->h = (long double) sqrt(
            (double) (pow2((vertices[3].x - vertices[0].x)) + pow2((vertices[3].y - vertices[0].y)) + pow2((vertices[3].z - vertices[0].z))));

    poli->V.x = (vertices[3].x-vertices[0].x)/poli->h;
    poli->V.y = (vertices[3].y-vertices[0].y)/poli->h;
    poli->V.z = (vertices[3].z-vertices[0].z)/poli->h;

    poli->rectangle = vertices;
}