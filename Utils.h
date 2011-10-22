#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Exceptions.h"


void SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface);


void updateFPS(unsigned int newTime);
double getFPS();

#endif