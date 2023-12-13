// elevmgr.h - Elevation Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#pragma once

#include "client/glclient/ztreemgr.h"
#include "api/ofsapi.h"
#include "api/elevmgr.h"

class Object;

class OFSAPI ElevationManager
{
public:
    ElevationManager() = default;
    ~ElevationManager() = default;

    // int16_t *loadElevationTile(int lod, int ilat, int ilng, double elevRes) const;
    int16_t *readElevationFile(int lod, int ilat, int ilng, double scale) const;

    bool getTileIndex(double lat, double lng, int lod, int &ilat, int &ilng) const;
    double getElevationData(glm::dvec3 loc, int reqlod = 0, elevTileList_t *elevTiles = nullptr,
        glm::dvec3 *normal = nullptr, int *lod = 0) const;
    // int16_t *getElevationData();

private:
    Object *object = nullptr;
    int elevMode = 1;

    const int elevGrid = ELEV_RES;
    const int elevStride = ELEV_STRIDE;
    
    double elevGrids;
    double elevScale = 1.0;

    zTreeManager *zTrees[2];

    // bool own = false;
    // int16_t *elev = nullptr;
    // int16_t *ggelev = nullptr;
};