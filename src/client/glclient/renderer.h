// renderer.h - Renderer API package
//
// Author:  Tim Stark
// Date:    Feb 14, 2024

#pragma once

//#include "glad/gl.h"

class glTexture;

class glRenderer
{
public:
#ifdef DEBUG
    void checkError(cchar_t *str);
#else
    inline void checkError(cchar_t *str) {}
#endif
    static void sync();

    static void pushRenderTarget(glTexture *tex);
    static void popRenderTarget();

    static void pushFlag(int flag, bool val);
    static void popFlag();
};