#include "raytracer.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t frameBufferMutex = PTHREAD_MUTEX_INITIALIZER;

rgb **frameBuffer;
int height;
int width;
int mirrorLevel;
int antiAliasingLevel;
vector eye;
rgb *background;
long double ia;
long double *window;
objectNode *objects;
lightNode *lights;

rgb** initFrameBuffer(int height, int width);
void * drawThread(void *data);
rgb getColor(vector a, vector b, int mirrorLevel);
int isDifferentColor(rgb colorTemp, int cont, rgb actual);
intersectionNode getFirstIntersection(vector a, vector b);
vector interToLight(vector p, vector Xi);
long double getFactAtt(long double c[], vector p, vector Xi);
long double getDistanceToLight(vector p, vector Xi);

rgb** draw(settings *sceneSettings, objectNode *pObjects, lightNode *pLights, int pThreadNumber) {
    height = sceneSettings->height;
    width = sceneSettings->width;
    mirrorLevel = sceneSettings->mirrorLevel;
    antiAliasingLevel = sceneSettings->antiAliasingLevel;
    eye = sceneSettings->eye;
    background = sceneSettings->background;
    ia = sceneSettings->Ia;
    window = sceneSettings->window;
    objects = pObjects;
    lights = pLights;

    frameBuffer = initFrameBuffer(height, width);
    pthread_t threads[pThreadNumber];
    for(int thread = 0; thread < pThreadNumber; thread++) {
        threadData *data = (threadData *) malloc(sizeof(threadData));
        data->threadId = thread;
        data->threadNum = pThreadNumber;
        pthread_create(&threads[thread], NULL, drawThread, (void *) data);
    }

    for(int thread = 0; thread < pThreadNumber; thread++) {
        pthread_join(threads[thread], NULL);
    }

    return frameBuffer;
}

void * drawThread(void *data) {
    threadData *tData = (threadData *) data;
    int cont, cont2;
    long double L, Xw, Yw, Zw, AAI, AAJ, sum;
    rgb color, colorTemp;
    vector d;
    for(int currentH = 0; currentH < height; currentH++) {
        for(int currentW = (int) tData->threadId; currentW < width; currentW += tData->threadNum) {
            color.r = 0;
            color.g = 0;
            color.b = 0;
            colorTemp.r = 0;
            colorTemp.g = 0;
            colorTemp.b = 0;
            cont2 = 0;
            sum = 1;
            do{
                cont = 0;
                for(AAI = 0; AAI <= 1; AAI += sum){
                    for(AAJ = 0; AAJ <= 1; AAJ += sum){
                        Xw = ((currentH + AAI) * (window[1] - window[0]) / height) + window[0];
                        Yw = ((currentW + AAJ) * (window[3] - window[2]) / width) + window[2];
                        Zw = 0.0;
                        d.x = (Xw - eye.x);
                        d.y = (Yw - eye.y);
                        d.z = (Zw - eye.z);

                        L = sqrt((double) (pow2(d.x) + pow2((d.y)) + pow2((d.z))));

                        d.x = d.x/L;
                        d.y = d.y/L;
                        d.z = d.z/L;

                        colorTemp = getColor(d, eye, mirrorLevel);
                        color.r += colorTemp.r;
                        color.g += colorTemp.g;
                        color.b += colorTemp.b;
                        cont++;
                    }
                }
                sum *= 0.5;
                cont2 += cont;
            }while(isDifferentColor(colorTemp, cont, color));

            color.r /= cont2;
            color.g /= cont2;
            color.b /= cont2;

            frameBuffer[currentH][currentW].r = color.r;
            frameBuffer[currentH][currentW].g = color.g;
            frameBuffer[currentH][currentW].b = color.b;
        }
    }
}

int isDifferentColor(rgb colorTemp, int cont, rgb actual) {
    long double r,g,b,r1,g1,b1, dr, dg, db;
    r1 = colorTemp.r;
    g1 = colorTemp.g;
    b1 = colorTemp.b;
    r = actual.r/cont;
    g = actual.g/cont;
    b = actual.b/cont;
    if(cont >= antiAliasingLevel)
        return 0;
    else{
        dr = sqrt((double) pow2(r - r1));
        dg = sqrt((double) pow2(g - g1));
        db = sqrt((double) pow2(b - b1));
        if(dr > DELTA || dg > DELTA || db > DELTA){
            return 1;
        }
    }
    return 0;
}

vector getXi(intersectionNode FI, vector a, vector b){
    vector L = {0, 0, 0};
    long double Xi, Yi, Zi;

    Xi = b.x + (FI.t*a.x);
    Yi = b.y + (FI.t*a.y);
    Zi = b.z + (FI.t*a.z);

    L.x = Xi;
    L.y = Yi;
    L.z = Zi;
    return L;
}

rgb getColor(vector a, vector b, int mirrorLevel) {
    intersectionNode firstIntersection, shadowIntersection;
    rgb color, tempColor, lightsColor;
    vector N, L, nXd, R, temp3;
    vector Xi = {0, 0, 0};
    vector temp2;
    vector temp;
    lightNode *k;
    long double I, E, factAtt, product;

    firstIntersection = getFirstIntersection(a, b);
    if(!firstIntersection.object) {
        color.r = background->r;
        color.g = background->g;
        color.b = background->b;
        return color;
    } else {
        nXd.x = -a.x;
        nXd.y = -a.y;
        nXd.z = -a.z;

        temp3.x = a.x;
        temp3.y = a.y;
        temp3.z = a.z;

        N = firstIntersection.object->getNormal(b, a, firstIntersection.t, firstIntersection.object);
        if(productoPunto(temp3,N) > DELTA){
            N.x = -N.x;
            N.y = -N.y;
            N.z = -N.z;
        }
        E = 0.0;
        I = 0.0;
        k = lights;
        int lightCount = 0;
        lightsColor.r = 0;
        lightsColor.g = 0;
        lightsColor.b = 0;
        while(k){
            Xi = getXi(firstIntersection, a, b);
            temp2.x = Xi.x;
            temp2.y = Xi.y;
            temp2.z = Xi.z;
            L = interToLight(k->center, temp2);
            factAtt = getFactAtt(k->c, k->center, temp2);
            product = productoPunto(N, L);
            if(product > DELTA){
                temp.x = L.x;
                temp.y = L.y;
                temp.z = L.z;
                shadowIntersection = getFirstIntersection(temp, temp2);
                if(!shadowIntersection.object || shadowIntersection.t > getDistanceToLight(k->center, temp2)){
                    I = I + (product * firstIntersection.object->ambient * factAtt * k->intensity);
                    R.x = (N.x * 2 * product) - L.x;
                    R.y = (N.y * 2 * product) - L.y;
                    R.z = (N.z * 2 * product) - L.z;
                    E = E + (expTo(productoPunto(R,nXd), firstIntersection.object->kn) * factAtt
                             * k->intensity) * firstIntersection.object->ks;
                    lightsColor.r += k->color.r * min(1, I);
                    lightsColor.g += k->color.g * min(1, I);
                    lightsColor.b += k->color.b * min(1, I);
                }
            }
            lightCount++;
            k = k->next;
        }
        lightsColor.r = lightsColor.r/(long double)lightCount;
        lightsColor.g = lightsColor.g/(long double)lightCount;
        lightsColor.b = lightsColor.b/(long double)lightCount;

        I = I + ia * firstIntersection.object->ambient;
        I = min(1, I);
        E = min(1, E);
        if(firstIntersection.object != NULL && firstIntersection.object->texture > 0){
            color = firstIntersection.object->getTexture(firstIntersection.object->texture, firstIntersection.t,
                                                              a, b, firstIntersection.object);
            color.r = I * color.r;
            color.g = I * color.g;
            color.b = I * color.b;
        }
        else{
            color.r = I * (firstIntersection.object->color.r + lightsColor.r)/2;
            color.g = I * (firstIntersection.object->color.g + lightsColor.g)/2;
            color.b = I * (firstIntersection.object->color.b + lightsColor.b)/2;
        }
        if(E>0){
            color.r = color.r + E*(1-color.r);
            color.g = color.g + E*(1-color.g);
            color.b = color.b + E*(1-color.b);
        }
        if(firstIntersection.object->o2 > DELTA && mirrorLevel > 0){
            product = productoPunto(N,temp3);
            temp.x = temp3.x - ((2 * product) * N.x);
            temp.y = temp3.y - ((2 * product) * N.y);
            temp.z = temp3.z - ((2 * product) * N.z);
            temp2.x = Xi.x;
            temp2.y = Xi.y;
            temp2.z = Xi.z;
            tempColor = getColor(temp, temp2, mirrorLevel - 1);
            color.r = firstIntersection.object->o1 * color.r +
                firstIntersection.object->o2 * tempColor.r;

            color.g = firstIntersection.object->o1 * color.g +
                firstIntersection.object->o2 * tempColor.g;

            color.b = firstIntersection.object->o1 * color.b +
                firstIntersection.object->o2 * tempColor.b;
        }
        return color;
    }
}

intersectionNode getFirstIntersection(vector a, vector b) {
    objectNode *this = objects;
    intersectionNode intersection = {0, NULL, NULL};
    intersectionNode tempIntersection;
    long double t = -1.0f;
    while(this){
        tempIntersection = this->getIntersection(a, b, this);

        if(t == -1.0f && tempIntersection.t > DELTA){
            intersection = tempIntersection;
            t = intersection.t;
        }
        else if(tempIntersection.t < t && tempIntersection.t > DELTA){
            intersection = tempIntersection;
            t = intersection.t;
        }
        this = this->next;
    }
    return intersection;
}

long double getDistanceToLight(vector p, vector Xi){
    vector L = {0, 0, 0};
    long double dist;

    L.x = p.x - Xi.x;
    L.y = p.y - Xi.y;
    L.z = p.z - Xi.z;

    dist = sqrt((double) (pow2(L.x) + pow2(L.y) + pow2(L.z)));
    return dist;
}

vector interToLight(vector p, vector Xi){
    vector L = {0, 0, 0};
    long double dist;

    L.x = p.x - Xi.x;
    L.y = p.y - Xi.y;
    L.z = p.z - Xi.z;

    dist = sqrt((double) (pow2(L.x) + pow2(L.y) + pow2(L.z)));

    L.x = (L.x)/dist;
    L.y = (L.y)/dist;
    L.z = (L.z)/dist;

    return L;
}

long double getFactAtt(long double c[], vector p, vector Xi){
    vector L = {0, 0, 0};
    long double fact_Att;
    long double dist;

    L.x = p.x - Xi.x;
    L.y = p.y - Xi.y;
    L.z = p.z - Xi.z;

    dist = sqrt((double) (pow2(L.x) + pow2(L.y) + pow2(L.z)));

    fact_Att = 1/(c[0]+(c[1]*dist)+(c[2]*pow2(dist)));

    return min(1, fact_Att);
}

rgb** initFrameBuffer(int height, int width) {
    rgb **frameBuffer = (rgb **) malloc(height * sizeof(rgb*));

    for(int currentH = 0; currentH < height; currentH++) {
        frameBuffer[currentH] = (rgb *) malloc(width * sizeof(rgb));
    }
    return frameBuffer;
}
