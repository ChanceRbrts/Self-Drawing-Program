#ifndef sdp_utils
#define sdp_utils

// This is just where we are going to include all the OpenGL/SDL stuff.
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
// This is how SDL installed on my Mac when I installed it.
// If it turns out that this is not where SDL is actually installed, then I'll add compiler flags.
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif