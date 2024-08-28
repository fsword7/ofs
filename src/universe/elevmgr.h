// elevmgr.h - Elevation Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#pragma once

#include "api/ofsapi.h"
#include "api/elevmgr.h"
#include "utils/ztreemgr.h"

class CelestialPlanet;

class OFSAPI ElevationManager
{
public:
    ElevationManager(CelestialPlanet *planet);
    ~ElevationManager() = default;

    void setup(const fs::path &folder);

    int16_t *readElevationFile(int lod, int ilat, int ilng, double scale) const;
    bool readElevationModFile(int lod, int ilat, int ilng, double elevScale, int16_t *elev) const;

    bool getTileIndex(double lat, double lng, int lod, int &ilat, int &ilng) const;
    double getElevationData(glm::dvec3 loc, int reqlod = 0, elevTileList_t *elevTiles = nullptr,
        glm::dvec3 *normal = nullptr, int *lod = 0) const;
    // int16_t *getElevationData();

    inline int getMode() const      { return elevMode; }

private:
    CelestialPlanet *object = nullptr;
    int elevMode = 1;

    const int elevGrid = ELEV_RES;
    const int elevStride = ELEV_STRIDE;
    
    double elevGrids;
    double elevScale = 1.0;

    mutable elevTileList_t localTiles;

    zTreeManager *zTrees[2] = { nullptr, nullptr};

    // bool own = false;
    // int16_t *elev = nullptr;
    // int16_t *ggelev = nullptr;
};