// texmgr.h - Texture Manager Package
//
// Author:  Tim Stark
// Date:    Sep 28, 2023

#pragma once

struct Texture
{
    Texture();
    ~Texture();
    
    inline void bind() const    { glBindTexture(GL_TEXTURE_2D, id); }
    inline void unbind() const  { glBindTexture(GL_TEXTURE_2D, 0); }

    void init();

    int32_t txWidth;
    int32_t txHeight;
    GLuint  id = 0;
    GLuint  fbo = 0;
    GLuint  rbo = 0;
};

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager() = default;

    void loadDDSTextureFromMemory(Texture **txImage, const uint8_t *buf, uint32_t nbuf, uint32_t flags);
    Texture *getTextureForRendering(int width, int height, uint32_t flags = 0);

private:

};