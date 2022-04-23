// ztreemgr.h - Orbiter's ztree manager package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#pragma once

#define ZTREE_NIL uint32_t(-1)

inline uint32_t FOURCC(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return (uint32_t(d) << 24) | (uint32_t(c) << 16) | (uint32_t(b) << 8) | uint32_t(a);
}

struct zTreeHeader
{
    uint32_t magic;         // File ID and version ('TX10')
    uint32_t size;          // header size [bytes]
    uint32_t flags;         // flags
    uint32_t dataOfs;       // data block offset (header + TOC)
    uint64_t dataLength;    // Length of compressed data block
    uint32_t nodeCount;     // Number of tree nodes
    uint32_t rootPos1;      // Index of LOD 0 tile
    uint32_t rootPos2;      // Index of LOD 1 tile
    uint32_t rootPos3;      // Index of LOD 2 tile
    uint32_t rootPos4[2];   // Index of LOD 3 tiles (two tiles)
};

struct zTreeNode
{
    uint64_t pos;           // File offset of node data
    uint32_t size;          // Data block length [bytes]
    uint32_t child[4];      // Index position of child nodes
};

class zTreeManager
{
public:
    zTreeManager() = default;
    ~zTreeManager();

    static zTreeManager *create(const fs::path &pname, cstr_t &tname);

    bool open(const fs::path &fname);
    int read(int lod, int lat, int lng, uint8_t **data);

protected:
    int32_t getIndex(int lod, int lat, int lng);
    uint32_t getDeflatedSize(uint32_t idx);
    uint32_t getInflatedSize(uint32_t idx);
    int inflateData(uint8_t *zdata, uint32_t zsize, uint8_t *udata, uint32_t usize);

private:
    std::ifstream zfile;

    zTreeHeader hdr;
    zTreeNode  *nodes;
};
