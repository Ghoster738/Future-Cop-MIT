#ifndef OPENGLES_2_DECLARATION
#define OPENGLES_2_DECLARATION

#include "Config.h"

#define GL_GLEXT_PROTOTYPES 1

#ifdef FORCE_FULL_OPENGL_2
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles2.h>
#endif

#endif
