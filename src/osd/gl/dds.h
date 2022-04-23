// dds.h - DirectDraw DDS format specification
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

// dwFlags
#define DDSD_CAPS         0x00000001 // Bit set required
#define DDSD_HEIGHT       0x00000002 // Bit set required
#define DDSD_WIDTH        0x00000004 // Bit set required
#define DDSD_PITCH        0x00000008 // Pitch for uncompressed texture
#define DDSD_PIXELFORMAT  0x00001000 // Bit seq required
#define DDSD_MIPMAPCOUNT  0x00020000 // Mipmapped texture
#define DDSD_LINEARSIZE   0x00080000 // Pitch for compressed texture
#define DDSD_DEPTH        0x00800000 // Depth texture

// dwCaps
#define DDCAPS_COMPLEX    0x00000008
#define DDCAPS_TEXTURE    0x00001000
#define DDCAPS_MIPMAP     0x00400000

// dwCaps2
#define DDCAPS2_CUBEMAP                0x00000200
#define DDCAPS2_CUBEMAP_POSTIVEX       0x00000400
#define DDCAPS2_CUBEMAP_NEGATIVEX      0x00000800
#define DDCAPS2_CUBEMAP_POSTIVEY       0x00001000
#define DDCAPS2_CUBEMAP_NEGATIVEY      0x00002000
#define DDCAPS2_CUBEMAP_POSTIVEZ       0x00004000
#define DDCAPS2_CUBEMAP_NEGATIVEZ      0x00008000
#define DDCAPS2_CUBEMAP_VOLUME         0x02000000

#define DDPF_ALPHAPIXELS   0x00000001
#define DDPF_ALPHA         0x00000002
#define DDPF_FOURCC        0x00000004
#define DDPF_RGB           0x00000040
#define DDPF_YUV           0x00000200
#define DDPF_LUMINANCE     0x00020000

struct ddsHeaderDXT10 {

};

struct ddsPixelFormat {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

struct ddsHeader {
    uint32_t  dwMagic;
    uint32_t  dwSize;
    uint32_t  dwFlags;
    uint32_t  dwHeight;
    uint32_t  dwWidth;
    uint32_t  dwPitchOrLinearSize;
    uint32_t  dwDepth;
    uint32_t  dwMipMapCount;
    uint32_t  dwReserved[11];
    ddsPixelFormat ddpf;
    uint32_t  dwCaps;
    uint32_t  dwCaps2;
    uint32_t  dwCaps3;
    uint32_t  dwCaps4;
    uint32_t  dwReserved2;
};

// class ddsLoader
// {
// public:
// 	ddsLoader() = default;
// 	~ddsLoader() = default;

//     Texture *load(std::string& fname);
//     Texture *load(uint8_t *data, uint32_t size);
// };

