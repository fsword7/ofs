// elevmgr.h - Elevation Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#pragma once

class ElevationManager
{
public:
    ElevationManager() = default;
    ~ElevationManager() = default;

    int16_t *loadElevationTile(int lod, int ilat, int ilng, double elevRes) const;

    double getElevation();
    int16_t *getElevationData();

private:
    int mode = 0;

    const int elevGrid = 256;
    const int elevStride = elevGrid+3;
    
    double elevRes = 1.0;

    // bool own = false;
    // int16_t *elev = nullptr;
    // int16_t *ggelev = nullptr;
};