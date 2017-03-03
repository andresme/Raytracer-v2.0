#ifndef SHARED_STRUCTS_H
#define SHARED_STRUCTS_H

typedef struct vector {
    long double x;
    long double y;
    long double z;
} vector;

typedef struct rgb {
     long double r;
     long double g;
     long double b;
} rgb;

typedef struct plane {
    vector N;
    long double A;
    long double B;
    long double C;
    long double D;
} plane;

typedef struct threadData {
    long threadId;
    int threadNum;
} threadData;

typedef struct settings {
    vector eye;
    long double *window;
    int height;
    int width;
    long double Ia;
    rgb* background;
    int antiAliasingLevel;
    int mirrorLevel;
} settings;

typedef struct textureStruct {
    int width;
    int height;
    rgb *fileInfo;
    char* name;
    struct textureStruct *next;
} textureStruct;

#endif
