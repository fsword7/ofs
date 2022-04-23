// texture.h - texture package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

#include "SDL2/SDL_image.h"

#include "osd/gl/context.h"
#include "osd/gl/dds.h"

inline uint32_t getFourCC(const char *code)
{
    return  ((uint32_t)code[3] << 24) |
            ((uint32_t)code[2] << 16) |
            ((uint32_t)code[1] << 8)  |
            ((uint32_t)code[0]);
}

class Texture
{
public:
    enum MipMapMode
    {
        NoMipMaps = 0,
        FixedMipMaps = 1,
        AutoMipMaps = 2
    };

    enum BorderMode
    {
        Warp = 0,
        BorderCLamp = 1,
        EdgeClamp = 2
    };

    Texture(uint32_t width, uint32_t height);
    ~Texture();

    int getFormatComponents(int format);
    int getDataSize(int w, int h, int format);

    void load();
    
    inline bool isCompressed() const { return compressed; }
    inline bool isLoaded() const     { return loadFlag; }
    inline void bind() const         { glBindTexture(GL_TEXTURE_2D, id); }
    inline void unbind() const       { glBindTexture(GL_TEXTURE_2D, 0); }

    static Texture *loadDDSFromMemory(uint8_t *data, size_t size);
    static Texture *loadDDSFromFile(fs::path &fname);
    static Texture *create(fs::path &fname);

    inline int pad(int n) const { return (n + 3) & ~0x03; }

private:
    uint32_t width, height;
    size_t   size;

    int      format;
    bool     compressed;
    uint32_t components = 0;

    uint8_t *data = nullptr;

    MipMapMode mipMode = NoMipMaps;
    BorderMode borderMode = EdgeClamp;

    bool loadFlag = false;
    GLuint id = 0;
};
