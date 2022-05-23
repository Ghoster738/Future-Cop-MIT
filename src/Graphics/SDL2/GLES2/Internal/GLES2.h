#ifndef OPENGLES_2_DECLARATION
#define OPENGLES_2_DECLARATION

#include "Config.h"

#define GL_GLEXT_PROTOTYPES 1

#ifdef GLEW_FOUND
#include <GL/glew.h>
#endif

#ifndef FORCE_FULL_OPENGL_2
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#endif
