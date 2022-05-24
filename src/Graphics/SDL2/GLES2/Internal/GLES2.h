#ifndef OPENGLES_2_DECLARATION
#define OPENGLES_2_DECLARATION

#include "Config.h"

#define GL_GLEXT_PROTOTYPES 1

#ifdef GLEW_FOUND
#include <GL/glew.h>
#endif

#include <SDL2/SDL_opengles2.h>

#ifndef FORCE_FULL_OPENGL_2
#define GLES2_SDL_GL_CONTEXT_SETTING SDL_GL_CONTEXT_PROFILE_ES
#else
#define GLES2_SDL_GL_CONTEXT_SETTING SDL_GL_CONTEXT_PROFILE_CORE
#endif

#endif
