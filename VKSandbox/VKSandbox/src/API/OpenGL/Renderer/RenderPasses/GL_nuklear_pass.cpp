#include "API/OpenGL/Renderer/GL_renderer.h"

#include "Input/Input.h"

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define NK_MEMSET memset
#define NK_MEMCPY memcpy
#define NK_STRLEN strlen
#define NK_STRNCPY strncpy
#define NK_SPRINTF sprintf
#define NK_VSNPRINTF vsnprintf
#define NK_VSCPRINTF vsnprintf
#define NK_STRICMP stricmp
#define NK_STRNICMP strnicmp
#define NK_STRTOK strtok
#define NK_TOLOWER tolower
#define NK_FOPEN fopen
#define NK_FCLOSE fclose
#define NK_FREAD fread
#define NK_FWRITE fwrite
#define NK_FSEEK fseek
#define NK_FTELL ftell
#define NK_SQRT sqrt
#define NK_SIN sin
#define NK_COS cos
#define NK_TAN tan
#define NK_ACOS acos
#define NK_ATAN2 atan2
#define NK_FLOOR floor
#define NK_CEIL ceil
#define NK_EXP exp
#define NK_POW pow
#define NK_LOG log
#define NK_MODF modf
#define NK_FABS fabs

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#define MAX_VERTEX_BUFFER 2 * 1024 * 1024  // 2MB for vertex buffer
#define MAX_ELEMENT_BUFFER 512 * 1024      // 512KB for element buffer

namespace OpenGLRenderer {

    //nk_context* g_context;

    void InitNuklear() {
        //GLFWwindow* window = (GLFWwindow*)GLFWIntegration::GetWindowPointer();
    }

    void NuklearPass() {

        static bool initilized = false;
        if (!initilized) {
            InitNuklear();
        }

    }
}