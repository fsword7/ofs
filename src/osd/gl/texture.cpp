// texture.cpp - texture package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/texture.h"

Texture::Texture(uint32_t w, uint32_t h)
: width(w), height(h)
{
    compressed = false;
}

Texture::~Texture()
{
    if (data != nullptr)
        delete data;
    if (id > 0)
        glDeleteTextures(1, &id);
}

int Texture::getFormatComponents(int format)
{
    switch (format)
    {
    case GL_RGBA:
        return 4;
    case GL_RGB:
        return 3;
    case GL_LUMINANCE_ALPHA:
        return 2;
    case GL_ALPHA:
    case GL_INTENSITY:
    case GL_LUMINANCE:
        return 1;

    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        return 3;
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return 4;

    default:
        return 0;
    }
}

int Texture::getDataSize(int w, int h, int format)
{
    switch (format)
    {
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return ((w + 3) / 4) * ((h + 3) / 4) * 8;

        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return ((w + 3) / 4) * ((h + 3) / 4) * 16;

        default:
            return h * pad(w * getFormatComponents(format));
    }
}

Texture *Texture::loadDDSFromMemory(uint8_t *data, size_t size)
{
    uint8_t   *ptr = data;
    ddsHeader *hdr = (ddsHeader *)data;
    int glFormat = -1;

    // Check validation
    if (hdr->dwMagic != getFourCC("DDS ") ||
        hdr->dwSize != sizeof(ddsHeader) - sizeof(uint32_t))
    {
        // Bad DDS header - aborted
        return nullptr;
    }
    ptr += sizeof(ddsHeader);

    if (hdr->ddpf.dwFourCC != 0)
    {
        char fourCC[5];

        *(uint32_t *)&fourCC - hdr->ddpf.dwFourCC;
        fourCC[4] = '\0';

        if (hdr->ddpf.dwFourCC == getFourCC("DXT1"))
            glFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        else if (hdr->ddpf.dwFourCC == getFourCC("DXT3"))
            glFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        else if (hdr->ddpf.dwFourCC == getFourCC("DXT5"))
            glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        else
        {
            // Unknown DDS format code - aborted
            return nullptr;
        }
    }

    Texture *texImage = new Texture(hdr->dwWidth, hdr->dwHeight);
    texImage->components = texImage->getFormatComponents(glFormat);
    texImage->format = glFormat;
    texImage->compressed = true;
    texImage->size = texImage->getDataSize(hdr->dwWidth, hdr->dwHeight, glFormat);
    texImage->data = new uint8_t[texImage->size];

    std::copy(ptr, ptr + texImage->size, texImage->data);

    return texImage;
}

Texture *Texture::loadDDSFromFile(fs::path &fname)
{
    uint8_t *data;
    uint32_t size;

    std::ifstream ddsFile(fname, std::ios::binary|std::ios::in);
    if (!ddsFile.is_open())
        return nullptr;
    
    ddsFile.seekg(0, std::ios::end);
    size = ddsFile.tellg();
    ddsFile.seekg(0, std::ios::beg);

    data = new uint8_t[size];
    ddsFile.read((char *)data, size);

    Texture *texImage = loadDDSFromMemory(data, size);
    delete [] data;

    return texImage;
}

void Texture::load()
{
    int target = GL_TEXTURE_2D;
    int bFlag = GL_CLAMP_TO_EDGE;

    glGenTextures(1, &id);
    bind();

    glTexParameteri(target, GL_TEXTURE_WRAP_S, bFlag);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, bFlag);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (isCompressed())
        glCompressedTexImage2D(target, 0, format, width, height, 0, size, data);
    else
        glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    if (mipMode == AutoMipMaps)
        glGenerateMipmap(target);

    loadFlag = true;
}

Texture *Texture::create(fs::path &fname)
{
    Texture *texImage = nullptr;

    if (fname.extension() == ".dds")
        return loadDDSFromFile(fname);
    else
    {
        SDL_Surface *image = IMG_Load(fname.string().c_str());
        if (image == nullptr)
            return nullptr;
        
        SDL_PixelFormat *format = image->format;
        int glFormat = -1;

        if (format->BytesPerPixel == 4)
        {
            if (format->Rshift == 24 && format->Aloss == 0)
                glFormat = GL_ABGR_EXT;
            else if (format->Rshift == 16 && format->Aloss == 8)
                glFormat = GL_BGRA;
            else if (format->Rshift == 16 && format->Ashift == 24)
                glFormat = GL_BGRA;
            else if (format->Rshift == 0 && format->Ashift == 24)
                glFormat = GL_RGBA;
        }
        else if (format->BytesPerPixel == 3)
        {
            if (format->Rshift == 16)
                glFormat = GL_BGR;
            else if (format->Rshift == 0)
                glFormat = GL_RGB;
        }
        else
        {

        }

        texImage = new Texture(image->w, image->h);
        texImage->components = format->BytesPerPixel;
        texImage->size = image->w * image->h * format->BytesPerPixel;
        texImage->data = new uint8_t[texImage->size];

        std::copy((uint8_t *)image->pixels, (uint8_t *)image->pixels + texImage->size, texImage->data);

        // All done, release surface now
        SDL_FreeSurface(image);
    }

    return texImage;
}
