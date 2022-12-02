#ifndef OPENGLES_2_DECLARATION
#define OPENGLES_2_DECLARATION

#include "Config.h"

#include <SDL2/SDL.h>

#ifdef GLEW_FOUND

// Simply include GLEW.
#include <GL/glew.h>

#else // If GLEW is not found

// This includes Core OpenGL Functionality.
#define GL_GLEXT_PROTOTYPES

#ifdef FORCE_FULL_OPENGL_2
#include <SDL2/SDL_opengl.h>
#else
#include <SDL2/SDL_opengles2.h>
#endif // FORCE_FULL_OPENGL_2

#endif // GLEW_FOUND

#endif
