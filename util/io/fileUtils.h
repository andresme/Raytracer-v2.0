#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "../../scene/object/objects.h"
#include "../../scene/object/light.h"

objectNode* readObjectsFromFile(char *fileName);
lightNode* readLightsFromFile(char *fileName);
settings* readSettingsFromFile(char *fileName);
rgb getTexel(long double u, long double v, int d, int h, objectNode *object);
void writeTofile(rgb **frameBuffer, char *fileName, int width, int height);

#endif
