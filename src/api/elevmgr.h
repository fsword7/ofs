// elevmgr.h - Surface (Elevation) package
//
// Author:  Tim Stark
// Date:    Oct 14, 2023

#pragma once

#define TILE_RES        256

// Elevation tile resolution
#define ELEV_RES        TILE_RES
#define ELEV_XRES       (TILE_RES+3)
#define ELEV_YRES       (TILE_RES+3)
#define ELEV_STRIDE     ELEV_XRES
#define ELEV_LENGTH     (ELEV_XRES*ELEV_YRES)

// Elevation file header
// Format type:
//     0 = flat (no data)
//     8 = unsigned byte (8-bit)
//    -8 = signed byte (8-bit)
//    16 = unsigned short (16-bit)
//   -16 = signed short (16-bit)

#pragma pack(push, 1)
struct elevHeader
{
    uint32_t code;                  // Code 'ELE1' in four CC format
    int      hdrSize;               // Header length (expected 76 bytes)
    int      format;                // Data format
    int      xgrd, ygrd;            // (X,Y) grids  (expected 259 x 259)
    int      xpad, ypad;            // (X,Y) pads   (expected 1 x 1)
    double   scale;                 // Elevation scale
    double   offset;                // Elevation offset
    double   latmin, latmax;        // Latitude range [rad]
    double   lngmin, lngmax;        // Longtitude range [rad]
    double   emin, emax, emean;     // Min, max, and mean elevation [m]
};
#pragma pack(pop)

struct ElevationTile
{
    int lod;
    int ilat, ilng;
    int nlat, nlng;

    double latmin, latmax;
    double lngmin, lngmax;

    // SurfaceTile *tile = nullptr;
    int16_t *data = nullptr;
    glm::dvec3 normal;

    int tgtlod;
    double lat0, lng0;
    double lastAccess;
};

using elevTileList_t = std::vector<ElevationTile>;