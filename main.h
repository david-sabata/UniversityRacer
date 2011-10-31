
#include <stdlib.h>
#include <exception>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <SDL/SDL.h>

#ifndef GLM_INCLUDED
#define GLM_INCLUDED
#include <glm/glm.hpp>
# ifndef USE_ONE
#	include <glm/gtc/matrix_projection.hpp>
# endif
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#ifdef USE_GLEE
# include <GL/GLee.h>
#else
# include <GL/glew.h>
#endif

#include <GL/glu.h>

#include "Exceptions.h"
#include "Utils.h"
#include "BaseApp.h"
#include "Game.h"
