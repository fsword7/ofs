// texmgr.cpp - Texture Manager Package
//
// Author:  Tim Stark
// Date:    Sep 28, 2023

#include "main/core.h"
#include "client.h"
#include "texmgr.h"

#include "SOIL2/SOIL2.h"

Texture::Texture()
{

}

Texture::~Texture()
{
    // if (txData != nullptr)
    //     delete txData;
    if (id > 0)
        glDeleteTextures(1, &id);
    if (fbo > 0)
        glDeleteFramebuffers(1, &fbo);
    if (rbo > 0)
        glDeleteRenderbuffers(1, &rbo);
}

void Texture::init()
{
    bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &txWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &txHeight);
    unbind();
}

void TextureManager::loadDDSTextureFromMemory(Texture **txImage, const uint8_t *buf, uint32_t nbuf, uint32_t flags)
{
    GLuint id;

    *txImage = new Texture();

    (*txImage)->id = SOIL_direct_load_DDS_from_memory((const unsigned char *)buf, nbuf, 0, 0, 0);
    if (*txImage == nullptr)
    {
        glLogger->info("{}\n", SOIL_last_result());
        return;
    }

    (*txImage)->init();
}

// For more information, check http://www.opengl-tutorial.org/fr/intermediate-tutorials/tutorial-14-render-to-texture/
Texture *TextureManager::getTextureForRendering(int w, int h, uint32_t flags)
{
    Texture *tx = new Texture;
    tx->txWidth = w;
    tx->txHeight = h;

    GLenum format = GL_RGBA;
    if (flags & SURF_NOALPHA)
        format = GL_RGB;
    
    GLint oldFB;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFB);

    glGenFramebuffers(1, &tx->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, tx->fbo);

    glGenTextures(1, &tx->id);
    glBindTexture(GL_TEXTURE_2D, tx->id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (flags & SURF_MIPMAPS)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tx->id, 0);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glGenRenderbuffers(1, &tx->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, tx->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tx->rbo);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        glLogger->fatal("Framebuffer Failure - code {}\n", status);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, oldFB);
        delete tx;
        return nullptr;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFB);

    return tx;
}