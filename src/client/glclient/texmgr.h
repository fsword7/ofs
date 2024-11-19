// texmgr.h - Texture Manager Package
//
// Author:  Tim Stark
// Date:    Sep 28, 2023

#pragma once

#include "api/graphics.h"

class TextureManager;
class glRenderer;

class glTexture : public Texture
{
    friend class TextureManager;
    friend class glRenderer;

public:
    glTexture() = default;
    glTexture(int id) : id(id) {}
    glTexture(int w, int h) : Texture(w, h) {}
    ~glTexture();
    
    inline void bind() const    { glBindTexture(GL_TEXTURE_2D, id); }
    inline void unbind() const  { glBindTexture(GL_TEXTURE_2D, 0); }

    inline GLuint getID() const     { return id; }
    inline GLuint getFBO() const    { return fbo; }
    inline GLuint getRBO() const    { return rbo; }

    inline void setID(int _id)      { id = _id; }
    inline void setFBO(int _fbo)    { fbo = _fbo; }
    inline void setRBO(int _rbo)    { rbo = _rbo; }

    void init();

private:
    GLuint  id = 0;
    GLuint  fbo = 0;
    GLuint  rbo = 0;
};

struct txEntry
{
    str_t fname;
    glTexture *txImage;
};

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager();

    glTexture *loadTexture(cstr_t &fname, int flags);
    glTexture *getTexture(cstr_t &fname, int flags);

    glTexture *loadDDSTextureFromMemory(const uint8_t *buf, uint32_t nbuf, uint32_t flags);
    glTexture *getTextureForRendering(int width, int height, uint32_t flags = 0);

    // Texture List Repository
    void addTexture(glTexture *tx, cstr_t &fname);
    glTexture *findTexture(cstr_t &fname);
    void clearTextureList();

private:
    std::vector<txEntry *> txList;
};