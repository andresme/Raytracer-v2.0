#ifndef SPHERE_H_
#define SPHERE_H_

#include <math.h>
#include "objects.h"
#include "../../util/math/mathUtils.h"

typedef struct sphere{
	vector* center;
	//plane* cortes;
	long double radius;
	long double radius2;
	//int cantCortes;
} sphere;


objectNode * addSphereO(int type, long double center[], long double radius, long double color[], long double amb, long double ks,
                        int kn, long double o1, long double o2, int cantCortes, vector* cortes, vector* nCortes, objectNode *objects);
sphere* createSphere(long double center[], long double radius/*, int cantCortes, plane* cortes*/);
void freeSphere(struct object *this);
intersectionNode* getIntersection(vector dir, vector anchor, objectNode *this);
vector* getNormal(vector eye, vector dir, long double t, objectNode *this);

#endif /* SPHERE_H_ */
