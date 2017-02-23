#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "util/struct/sharedStructs.h"
#include "scene/raytracer/raytracer.h"
#include "util/io/fileUtils.h"

int main (int argc, char **argv) {
    char *objectsFileName = NULL;
    char *lightsFileName = NULL;
    char *settingsFileName = NULL;
    char *outputFileName = NULL;
    int threadNumber = 1;

    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "o:l:s:t:f:")) != -1) {
        switch (c) {
            case 'o':
                objectsFileName = optarg;
                break;
            case 'l':
                lightsFileName = optarg;
                break;
            case 's':
                settingsFileName = optarg;
                break;
            case 't':
                threadNumber = atoi(optarg);
                break;
            case 'f':
                outputFileName = optarg;
                break;
            case '?':
                if (optopt == 'o') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else if (optopt == 'l') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else if (optopt == 's') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else if (optopt == 't') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else if (optopt == 'f') {
                    printf("Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    printf("Unknown option `-%c'.\n", optopt);
                } else {
                    printf("Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                printf("Aborting...");
                abort ();
        }
    }

    if(objectsFileName == NULL) {
        printf("-of option is required");
        exit(-1);
    }
    if(lightsFileName == NULL) {
        printf("-lf option is required");
        exit(-1);
    }
    if(settingsFileName == NULL) {
        printf("-sf option is required");
        exit(-1);
    }
    if(outputFileName == NULL) {
        printf("-o option is required");
        exit(-1);
    }

    printf ("object file name = %s\nlights file name = %s\nsettings file name = %s\nnumber of threads: %d\noutput: %s\n",
            objectsFileName, lightsFileName, settingsFileName, threadNumber, outputFileName);

    settings *pictureSettings = readSettingsFromFile(settingsFileName);
    objectNode *objects = readObjectsFromFile(objectsFileName);
    lightNode *lights = readLightsFromFile(lightsFileName);

    clock_t begin = clock();
    rgb** picture = draw(pictureSettings, objects, lights, threadNumber);
    clock_t end = clock();

    free(pictureSettings->window);
    free(pictureSettings->background);
    objectNode *current = objects;
    while(current != NULL) {
        objectNode *temp = current;
        current = current->next;
        temp->deleteObject(temp->info);
    }
    lightNode *currentL = lights;
    while(currentL != NULL) {
        lightNode *temp = currentL;
        currentL = currentL->next;
        free(temp->color);
        free(temp);
    }
    writeTofile(picture, outputFileName, pictureSettings->width, pictureSettings->height);
    for(int i = 0; i < pictureSettings->height; i++) {
        free(picture[i]);
    }
    free(picture);
    free(pictureSettings);

    double time_spent = ((double)(end - begin) / CLOCKS_PER_SEC) / threadNumber;
    printf("time raytracing: %lf seconds.\n", time_spent);
    return 0;
}
