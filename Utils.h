#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <SDL/SDL.h>
#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include "Exceptions.h"

#define M_PI 3.14159265358979323846264338327950288419716939937510

void updateFPS(unsigned int newTime);
double getFPS();

#endif
