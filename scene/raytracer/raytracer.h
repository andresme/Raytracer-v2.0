#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "../object/objects.h"
#include "../object/sphere.h"
#include "../../util/math/mathUtils.h"
#include "../../util/struct/sharedStructs.h"
#include "../object/light.h"

rgb** draw(settings *sceneSettings, objectNode *pObjects, lightNode *pLights, int pThreadNumber);

#endif
