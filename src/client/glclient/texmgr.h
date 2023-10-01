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
    GLuint id = 0;

};

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager() = default;

private:

};