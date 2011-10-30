#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <stdexcept>
#include <string>

#include <SDL/SDL.h>

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Exception classes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Replacement for gluErrorString
const char * getGlErrorString(GLenum error);


struct SDL_Exception : public std::runtime_error
{
    SDL_Exception() throw() : std::runtime_error(std::string("SDL : ") + SDL_GetError()) {}
};


struct GL_Exception : public std::runtime_error
{
    GL_Exception(const GLenum error = glGetError()) throw()
        : std::runtime_error(std::string("OpenGL : ") + (const char*)getGlErrorString(error)) {}
    GL_Exception(const char * text, const GLenum error = glGetError()) throw()
        : std::runtime_error(std::string("OpenGL : ") + text + " - " + (const char*)getGlErrorString(error)) {}
};


#endif
