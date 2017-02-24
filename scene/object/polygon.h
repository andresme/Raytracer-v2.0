#ifndef RT2_POLYGON_H
#define RT2_POLYGON_H

#include "objects.h"

typedef struct vertex2D{
    long double U,V;
} vertex2D;

typedef struct polygon{
    int cantVertex, flag;
    long double w, h;
    vector *vertices, *rectangle, U, V;
    plane plano;
    vertex2D *vertices2D;
} polygon;

objectNode* addPolygonO(int type, vector* vertices, int cantidad, long double color[], long double amb, long double ks,
                        int kn, long double o1, long double o2, int style, char *imag, objectNode *objects);

polygon* createPolygon(vector* vertices, int cantidad);
void freePolygon(struct object *this);
vector getNormalPolygon(vector eye, vector dir, long double t, objectNode *this);
intersectionNode getIntersectionPolygon(vector dir, vector anchor, objectNode *object);
rgb getTexturePolygon(int style, long double t, vector dir, vector anchor, objectNode *object);


long double* calcIntersecPlane(vector dir, vector eye, plane plano);
long double* calcIntersecPlane0(vector dir, vector anchor, plane plano);
void flatVertex(polygon *newPolygon, int cantidad, vector *vertices);
int pointExists(long double* inter, objectNode *object);
plane createPlane(vector *vertices);
plane createPlane0(vector vertice, vector normal);


#endif //RT2_POLYGON_H
