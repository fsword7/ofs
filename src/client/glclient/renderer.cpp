// renderer.cpp - Renderer API package
//
// Author:  Tim Stark
// Date:    Feb 14, 2024

#include "main/core.h"
#include "engine/player.h"
#include "universe/universe.h"
#include "client.h"
#include "texmgr.h"
#include "renderer.h"
#include "skpad.h"
#include "scene.h"
#include "surface.h"
#include "vobject.h"
#include "vbody.h"

struct fbSaveParam
{
    GLint fbo;
    GLint viewport[4];
};

std::vector<fbSaveParam> lastFBParams;

#ifdef DEBUG
void checkError(cchar_t *str)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
        glogger->debug("OGL Error: {} - {}\n", str, err);
}
#endif

void glRenderer::sync()
{
}

void glRenderer::pushRenderTarget(glTexture *tex)
{
    assert(tex != nullptr);

    // Save current FBO and viewport parameters
    fbSaveParam save;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &save.fbo);
    glGetIntegerv(GL_VIEWPORT, save.viewport);
    lastFBParams.push_back(save);

    if (tex->fbo > 0)
    {
        // Bind existing framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, tex->fbo);
    }
    else
    {
        // Set up new framebuffer
        glGenFramebuffers(1, &tex->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, tex->fbo);
        glBindTexture(GL_TEXTURE_2D, tex->id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->id, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBuffers);

        switch(glCheckFramebufferStatus(GL_FRAMEBUFFER))
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        case GL_FRAMEBUFFER_UNSUPPORTED:
            break;
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        }
    }

    glViewport(0, 0, tex->getWidth(), tex->getHeight());
}

void glRenderer::popRenderTarget()
{
    fbSaveParam save = lastFBParams.back();
    lastFBParams.pop_back();

    // Restore previous framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, save.fbo);
    glViewport(save.viewport[0], save.viewport[1], save.viewport[2], save.viewport[3]);
}

void glRenderer::pushFlag(int flag, bool val)
{
}

void glRenderer::popFlag()
{
}