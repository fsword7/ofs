// renderer.h - Renderer API package
//
// Author:  Tim Stark
// Date:    Feb 14, 2024

#pragma once

#include "glad/gl.h"

class Texture;

namespace gl
{

#ifdef DEBUG
    void checkError(cchar_t *str);
#else
    inline void checkError(cchar_t *str) {}
#endif

    void pushRenderTarget(Texture *tex);
    void popRenderTarget();
};