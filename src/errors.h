#pragma once

#include <SDL_opengles2.h>
#include <string>
#include <iostream>
#include <fstream>

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
