#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <iterator>
#include <fstream>
#include <iostream>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "Exceptions.h"

// Load whole file and return it as std::string
std::string loadFile(const char * const file);

// Info log contains errors and warnings from shader compilation
std::string getShaderInfoLog(const GLuint shader);

// Info log contains errors and warnings from shader linking
std::string getProgramInfoLog(const GLuint program);

GLuint CompileShader(const GLenum type, const char * source);

GLuint LinkShader(size_t count, ...);

void SurfaceImage2D(GLenum target, GLint level, GLint internalformat, SDL_Surface * surface);


#endif