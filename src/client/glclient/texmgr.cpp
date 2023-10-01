// texmgr.cpp - Texture Manager Package
//
// Author:  Tim Stark
// Date:    Sep 28, 2023

#include "main/core.h"
#include "client.h"
#include "texmgr.h"

Texture::Texture()
{

}

Texture::~Texture()
{
    // if (txData != nullptr)
    //     delete txData;
    if (id > 0)
        glDeleteTextures(1, &id);
}

void Texture::init()
{
    bind();
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &txWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &txHeight);
    unbind();
}