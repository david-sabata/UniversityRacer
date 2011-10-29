#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Exceptions.h"

#define M_PI 3.14159265358979323846264338327950288419716939937510

void SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface);


void updateFPS(unsigned int newTime);
double getFPS();

#endif