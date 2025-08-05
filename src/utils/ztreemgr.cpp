// ztreemgr.cpp - Orbiter's ztree manager package
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "ztreemgr.h"

#include <unistd.h>
#include <zlib.h>

zTreeManager::~zTreeManager()
{
    if (zfile.is_open())
        zfile.close();
}

zTreeManager *zTreeManager::create(const fs::path &pname, cstr_t &tname)
{
    zTreeManager *ztree = new zTreeManager();

    fs::path fname = pname / (tname + ".tree");
    if (ztree->open(fname))
        return ztree;

    delete ztree;
    return nullptr;
}

bool zTreeManager::open(const fs::path &fname)
{
    ofsLogger->info("Opening file {}...\n", fname.string());
    zfile.open(fname.c_str(), std::ios::in|std::ios::binary);
    if (!zfile.is_open())
    {
        ofsLogger->info("Failed to open: {}\n", strerror(errno));
        return false;
    }

    zfile.seekg(0, zfile.end);
    auto fileSize = zfile.tellg();
    zfile.seekg(0, zfile.beg);
    // logger->info("File length = {} bytes\n", fileSize);

    zfile.read((char *)&hdr, sizeof(hdr));
    if (zfile.fail())
    {
        zfile.close();
        return false;
    }

    if (hdr.magic != FOURCC('T', 'X', 1, 0) || hdr.size != sizeof(hdr) ||
        hdr.dataLength + hdr.dataOfs != fileSize)
    {
        zfile.close();
        return false;
    }

    nodes = new zTreeNode[hdr.nodeCount];
    zfile.read((char *)nodes, sizeof(zTreeNode)*hdr.nodeCount);
    if (zfile.fail())
    {
        zfile.close();
        if (nodes != nullptr)
            delete [] nodes;
        nodes = nullptr;
        return false;
    }

    ofsLogger->info("Succesfully opened file\n");
    return true;
}

int32_t zTreeManager::getIndex(int lod, int lat, int lng)
{
    int32_t idx = ZTREE_NIL;

    switch (lod)
    {
    case 1: idx = hdr.rootPos1;         break;
    case 2: idx = hdr.rootPos2;         break;
    case 3: idx = hdr.rootPos3;         break;
    case 4: idx = hdr.rootPos4[lng];    break;

    default:
        int32_t pidx = getIndex(lod-1, lat/2, lng/2);
        if (pidx == ZTREE_NIL)
            break;
        int cidx = ((lat & 1) << 1) | (lng & 1);
        idx = nodes[pidx].child[cidx];
        break;
    }

    // ofsLogger->info("ztree(index): lod {} lat {} lng {} -> index {}\n",
    //     lod, lat, lng, idx);

    return idx;
}

uint32_t zTreeManager::getDeflatedSize(uint32_t idx)
{
    return uint32_t(((idx < hdr.nodeCount - 1) ? nodes[idx+1].pos : hdr.dataLength) - nodes[idx].pos);
}

uint32_t zTreeManager::getInflatedSize(uint32_t idx)
{
    return nodes[idx].size;
}

int zTreeManager::inflateData(uint8_t *zdata, uint32_t zsize, uint8_t *udata, uint32_t usize)
{
    z_stream zInfo = { 0 };
    int nerr, nret = -1;

    zInfo.zalloc = Z_NULL;
    zInfo.zfree = Z_NULL;
    zInfo.opaque = Z_NULL;

    zInfo.next_in = zdata;
    zInfo.avail_in = zsize;

    zInfo.next_out = udata;
    zInfo.avail_out = usize;

    nerr = inflateInit(&zInfo);
    if (nerr == Z_OK)
    {
        nerr = inflate(&zInfo, Z_FINISH);
        if (nerr == Z_STREAM_END)
            nret = (int)zInfo.total_out;
    }
    inflateEnd(&zInfo);

    return nret;
}

int zTreeManager::read(int lod, int lat, int lng, uint8_t **data, bool debug)
{
    uint32_t  idx = getIndex(lod, lat, lng);
    uint8_t   *zdata, *udata;
    uint32_t   zsize, usize;
    int        res;

    if (debug) ofsLogger->debug("Reading LOD {} Latitude {} Longitude {} Index {}\n",
        lod, lat, lng, idx);

    if (idx == ZTREE_NIL)
    {
        if (debug) ofsLogger->debug("Data not found - NIL data\n");
        *data = nullptr;
        return 0;
    }

    zsize = getDeflatedSize(idx);
    if (zsize == 0)
    {
        if (debug) ofsLogger->debug("Uncompressed failed - aborted.\n");
        *data = nullptr;
        return 0;
    }
    usize = getInflatedSize(idx);

    if (debug) ofsLogger->debug("Uncompressing {} -> {} bytes\n", zsize, usize);
    
    zdata = new uint8_t[zsize];
    udata = new uint8_t[usize];

    zfile.seekg(nodes[idx].pos + hdr.dataOfs, zfile.beg);
    zfile.read((char *)zdata, zsize);
    res = inflateData(zdata, zsize, udata, usize);
    if (res != usize)
    {
        delete [] udata;
        udata = nullptr;
    }
    delete [] zdata;

    *data = udata;
    return res;
}
