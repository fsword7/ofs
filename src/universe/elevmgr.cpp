// elevmgr.cpp - Elevation Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#include "main/core.h"
#include "universe/surfmgr.h"
#include "universe/elevmgr.h"


int16_t *ElevationManager::loadElevationTile(int lod, int ilat, int ilng, double elevRes) const
{
    int16_t *elev = nullptr;

    // if (mode > 0)
    // {
    //     const int ndat = elevStride*elevStride;

    //     if (zTrees[0] != nullptr)
    //     {
    //         int szData = zTrees[0]->read(lod, ilat, ilng, &data);
    //         if (data != nullptr)
    //         {

    //         }
    //     }
    // }

    return elev;
}

double ElevationManager::getElevation()
{
    double elev = 0.0;

    if (mode > 0)
    {

    }

    return elev * elevRes;
}



int16_t *ElevationManager::getElevationData()
{
    return nullptr;
}