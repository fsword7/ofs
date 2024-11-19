// texmgr.cpp - Texture Manager Package
//
// Author:  Tim Stark
// Date:    Sep 28, 2023

#include "main/core.h"
#include "client.h"
#include "renderer.h"
#include "texmgr.h"

#include "SOIL2/SOIL2.h"

glTexture::~glTexture()
{
    // if (txData != nullptr)
    //     delete txData;
    if (id > 0)
        glDeleteTextures(1, &id);
    if (fbo > 0)
        glDeleteFramebuffers(1, &fbo);
    if (rbo > 0)
        glDeleteRenderbuffers(1, &rbo);\
}

void glTexture::init()
{
    bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &txWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &txHeight);
    unbind();
}

TextureManager::~TextureManager()
{
    clearTextureList();
}

void TextureManager::addTexture(glTexture *tx, cstr_t &fname)
{
    txEntry *entry = new txEntry;

    entry->txImage = tx;
    entry->fname = fname;
    txList.push_back(entry);
}

glTexture *TextureManager::findTexture(cstr_t &fname)
{
    for (auto &entry : txList)
        if (entry->fname == fname)
            return entry->txImage;
    return nullptr;
}

void TextureManager::clearTextureList()
{
    for (auto &entry : txList)
        delete entry;
    txList.clear();
}

glTexture *TextureManager::loadTexture(cstr_t &fname, int flags)
{
    uint32_t sflags = SOIL_FLAG_TEXTURE_REPEATS;
    if ((flags & 4) == 0)
        sflags |= SOIL_FLAG_MIPMAPS;

    GLuint id = SOIL_load_OGL_texture(fname.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, sflags);

    glTexture *tx = nullptr;
    if (id != 0)
    {
        tx = new glTexture(id);
        tx->init();
    }
    return tx;
}

glTexture *TextureManager::getTexture(cstr_t &fname, int flags)
{
    glTexture *tx = findTexture(fname);
    if (tx != nullptr)
        return tx;
    tx = loadTexture(fname, flags);
    if (tx != nullptr)
        addTexture(tx, fname);
    return tx;
}

glTexture *TextureManager::loadDDSTextureFromMemory(const uint8_t *buf, uint32_t nbuf, uint32_t flags)
{
    GLuint id = SOIL_direct_load_DDS_from_memory((const unsigned char *)buf, nbuf, 0, 0, 0);
    if (id == 0)
    {
        glLogger->info("{}\n", SOIL_last_result());
        return nullptr;
    }

    glTexture *txImage = new glTexture();
    txImage->setID(id);
    txImage->init();

    return txImage;
}

// For more information, check http://www.opengl-tutorial.org/fr/intermediate-tutorials/tutorial-14-render-to-texture/
glTexture *TextureManager::getTextureForRendering(int w, int h, uint32_t flags)
{
    glTexture *tx = new glTexture(w, h);

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