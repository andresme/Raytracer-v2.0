#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fileUtils.h"
#include "../math/mathUtils.h"
#include "../../scene/object/light.h"
#include "../../scene/object/sphere.h"
#include "../../scene/object/cone.h"
#include "../../scene/object/disc.h"
#include "../../scene/object/cylinder.h"
#include "../../scene/object/polygon.h"

textureStruct* textureFromFile(char *fileName, textureStruct* texture);

textureStruct *textures = NULL;

void loadTexture(char *fileName) {
    int found = 0;
    textureStruct* texture = (textureStruct *) malloc(sizeof(texture));
    texture->name = (char *) malloc((strlen(fileName)+1)*sizeof(char));
    strcpy(texture->name, fileName);
    printf("Texture name: '%s':'%s'\n", texture->name, fileName);
    texture->next = NULL;
    if(textures == NULL) {
        textures = textureFromFile(texture->name, texture);
    } else {
        textureStruct* temp = textures;
        while(temp != NULL) {
            if(strcmp(fileName, temp->name) == 0) {
                found = 1;
                break;
            }
            temp = temp->next;
        }
        if(!found) {
            texture = textureFromFile(fileName, texture);
            texture->next = textures;
            textures = texture;
        }
    }
}


textureStruct* textureFromFile(char *fileName, textureStruct* texture) {
    FILE *fptr;
    int height, width;
    if ((fptr = fopen(fileName,"r")) != NULL){
        printf("Cargando: '%s'\n", fileName);
        fread(&width,sizeof(int),1,fptr);
        width = FIX(width);

        fread(&height,sizeof(int),1,fptr);
        height = FIX(height);

        texture->width = width;
        texture->height = height;

        texture->fileInfo = (rgb *) malloc(width*height*sizeof(rgb));

        for(int i = 0; i < width*height; i++){
            rgb texel = {0, 0, 0};
            fgetc(fptr);//alpha
            texel.r =(double)fgetc(fptr)/255;
            texel.g =(double)fgetc(fptr)/255;
            texel.b =(double)fgetc(fptr)/255;
            texture->fileInfo[i].r = texel.r;
            texture->fileInfo[i].g = texel.g;
            texture->fileInfo[i].b = texel.b;
        }
        fclose(fptr);
    } else{
        printf("No se encontro el archivo de imagen '%s'\n", fileName);
    }
    return texture;
}

objectNode* readObjectsFromFile(char *fileName) {
    vector *vertices, *nCorte;
    long double radius,amb,ks,color[3],center[3],Q[3],p[2],start[2], N[3], f1[3], f2[3], radius2, G[3];
    long double matriz[10];
    long double K, o1, o2;
    char info[256], *attrib, *imag;
    int contador = 0,type = 0, kn = 0, cantVertex = 0, cantCortes = 0, style = 0;

    FILE* fileptr = fopen(fileName, "rt");

    objectNode * objects = NULL;

    while(fgets(info, 256, fileptr) != NULL){ //Leer el archivo completo
        if(strlen(info) > 1){ //si el elemento leido no esta vacio
            attrib = strtok(info, ":");
            //Obtener la informacion del archivo
            if(!strcmp(attrib, "center")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &center[contador]);
                }
            }
                //Tipos de figura
            else if(!strcmp(attrib,"sphere"))
                type = 1;
            else if(!strcmp(attrib,"cone"))
                type = 2;
            else if(!strcmp(attrib,"disc"))
                type = 3;
            else if(!strcmp(attrib,"cylinder"))
                type = 4;
            else if(!strcmp(attrib,"polygon")){
                type = 5;
                contador = 0;
            }
            else if(!strcmp(attrib,"elipse"))
                type = 6;
            else if(!strcmp(attrib,"quadric"))
                type = 7;
            else if(!strcmp(attrib, "color")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &color[contador]);
                }
            }
            else if(!strcmp(attrib, "N")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &N[contador]);
                }
            }
            else if(!strcmp(attrib, "Matriz")){
                for(contador = 0; contador < 10; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &matriz[contador]);
                }
            }
            else if(!strcmp(attrib, "Q")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &Q[contador]);
                }
            }
            else if(!strcmp(attrib, "texFile")){
                attrib = strtok(NULL, ":");
                imag = (char *) malloc((strlen(attrib)+1)*sizeof(char));
                strcpy(imag, attrib);
                imag[strlen(attrib)-1] = '\0';
                loadTexture(imag);
            }
            else if(!strcmp(attrib, "G")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &G[contador]);
                }
            }
            else if(!strcmp(attrib, "f1")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &f1[contador]);
                }
            }
            else if(!strcmp(attrib, "f2")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &f2[contador]);
                }
            }
            else if(!strcmp(attrib, "prop")){
                for(contador = 0; contador < 2; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &p[contador]);
                }
            }
            else if(!strcmp(attrib, "start")){
                for(contador = 0; contador < 2; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &start[contador]);
                }
            }
            else if(!strcmp(attrib,"amb")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &amb);
            }
            else if(!strcmp(attrib,"o1")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &o1);
            }
            else if(!strcmp(attrib,"o2")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &o2);
            }
            else if(!strcmp(attrib,"radius")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &radius);
            }
            else if(!strcmp(attrib,"K")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &K);
            }
            else if(!strcmp(attrib,"radius2")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &radius2);
            }
            else if(!strcmp(attrib,"ks")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &ks);
            }
            else if(!strcmp(attrib,"kn")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &kn);
            }
            else if(!strcmp(attrib,"cantV")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &cantVertex);
                vertices = (vector*) malloc(cantVertex*sizeof(vector));
                contador = 0;
            }
            else if(!strcmp(attrib,"style")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &style);
            }
            else if(!strcmp(attrib,"vertex")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &vertices[contador].y);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &vertices[contador].x);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &vertices[contador].z);
                contador++;
            }
            else if(!strcmp(attrib,"cantCortes")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &cantCortes);
                if(cantCortes == 0){
                    nCorte = NULL;
                    vertices = NULL;
                }
                else{
                    nCorte = (vector*) malloc(cantCortes*sizeof(vector));
                    vertices = (vector*) malloc(cantCortes*sizeof(vector));
                }
                contador = 0;
            }
            else if(!strcmp(attrib,"nCorte")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &nCorte[contador].y);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &nCorte[contador].x);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &nCorte[contador].z);
            }
            else if(!strcmp(attrib, "//")){
                attrib = strtok(NULL, "\n");
            }
            else if(!strcmp(attrib,"add")){
                switch(type){
                    case 1:
                        objects = addSphereO(type,center,radius,color,amb, ks, kn, o1, o2, cantCortes, vertices, nCorte, objects);
                        break;
                    case 2:
                        objects = addConeO(type, center, Q, start, p, color, amb, ks, kn, o1, o2, objects);
                        break;
                    case 3:
                        objects = addDiscO(type, center, N, radius, radius2, color, amb, ks, kn, o1, o2, objects);
                        break;
                    case 4:
                        objects = addCylinderO(type, center, Q, G, start, radius, color, amb, ks, kn, o1, o2, style, objects);
                        break;
                    case 5:
                        objects = addPolygonO(type, vertices, cantVertex, color, amb, ks, kn, o1, o2, style, imag, objects);
                        break;
//                    case 7:
//                        addQuadricO(type, matriz, color, amb, ks, kn, o1, o2, cantCortes, vertices, nCorte);
//                        break;
                }
            }
        }
    }
    fclose(fileptr);
    return objects;
}

lightNode* readLightsFromFile(char *fileName) {
    long double intensity = 1, center[3], c[3],color[3];
    char info[40], *attrib;
    int contador;

    lightNode *lights = NULL;

    FILE* fileptr = fopen(fileName, "rt");

    while(fgets(info, 40, fileptr) != NULL){ //Leer el archivo completo
        if(strlen(info) > 1){ //si el elemento leido no esta vacio
            attrib = strtok(info, ":");
            if(!strcmp(attrib, "center")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &center[contador]);
                }
            }
            else if(!strcmp(attrib, "color")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &color[contador]);
                }
            }
            else if(!strcmp(attrib, "c")){
                for(contador = 0; contador < 3; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &c[contador]);
                }
            }
            else if(!strcmp(attrib,"inten")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &intensity);
            }
            else if(!strcmp(attrib,"add"))
                lights = addLight(center, intensity, c, color, lights);
        }
    }
    fclose(fileptr);
    return lights;
}

settings* readSettingsFromFile(char *fileName) {
    char info[40], *attrib;
    int contador;

    settings *sceneSettings = (settings *) malloc(sizeof(settings));
    sceneSettings->background = (rgb *) malloc(sizeof(rgb));
    sceneSettings->window = (long double *) malloc(4 * sizeof(long double));

    FILE* fileptr = fopen(fileName, "rt");

    while(fgets(info, 40, fileptr) != NULL){ //Leer el archivo completo
        if(strlen(info) > 1){ //si el elemento leido no esta vacio
            attrib = strtok(info, ":");
            if(!strcmp(attrib,"height")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &sceneSettings->height);
            }
            else if(!strcmp(attrib,"width")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d",&sceneSettings->width);
            }
            else if(!strcmp(attrib,"Ia")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->Ia);
            }
            else if(!strcmp(attrib,"anti")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &sceneSettings->antiAliasingLevel);
            }
            else if(!strcmp(attrib,"mirror")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%d", &sceneSettings->mirrorLevel);
            }
            else if(!strcmp(attrib,"background")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->background->r);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->background->g);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->background->b);
            }
            else if(!strcmp(attrib,"eye")){
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->eye.x);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->eye.y);
                attrib = strtok(NULL, ":");
                sscanf (attrib, "%Lg", &sceneSettings->eye.z);
            }
            else if(!strcmp(attrib,"window")){
                for(contador = 0; contador < 4; contador++){
                    attrib = strtok(NULL, ":");
                    sscanf (attrib, "%Lg", &sceneSettings->window[contador]);
                }
            }
        }
    }
    fclose(fileptr);
    return sceneSettings;
}

rgb getTexel(long double u, long double v, int d, int h, objectNode *object){
    FILE *fptr;
    rgb texel = {0, 0, 0};
    int height, width, calc, found = 0;

    textureStruct* temp = textures;
    while(temp != NULL) {
        if(strcmp(object->textureFile, temp->name) == 0) {
            found = 1;
            break;
        }
        temp = temp->next;
    }
    if(found) {
        width = temp->width;
        height = temp->height;

        if(object->texture == 1){
            v = (long double) (d%width)/width;
            u = (long double) (h%height)/height;
        }

        u =  u*height;
        v =  v*width;

        calc = (int) (u*width+v);

        texel = temp->fileInfo[calc];
    } else{
        printf("No se encontro el archivo de imagen %s\n",object->textureFile);
    }
    return texel;
}

void writeTofile(rgb **frameBuffer, char *fileName, int width, int height) {
    int i,j;
    FILE *fptr;
    int fixedWidth,fixedHeight;

    if ((fptr = fopen(fileName,"w")) != NULL) {

        fixedWidth = width;
        fixedWidth = FIX(fixedWidth);
        fixedHeight = height;
        fixedHeight = FIX(fixedHeight);

        fwrite(&fixedWidth, sizeof(unsigned int),1,fptr);
        fwrite(&fixedHeight, sizeof(unsigned int),1,fptr);


        for (i=height; i>0; i--) {
            for (j=0;j<width;j++) {
                fputc(255, fptr);
                fputc(frameBuffer[i-1][j].r * 255, fptr);
                fputc(frameBuffer[i-1][j].g * 255, fptr);
                fputc(frameBuffer[i-1][j].b * 255, fptr);
            }
        }
    }
    else
        printf("Error escribiendo archivo");
    fclose(fptr);
}