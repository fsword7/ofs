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
        logger->info("{}\n", SOIL_last_result());
        return;
    }

    (*txImage)->init();
}