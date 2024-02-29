// elevmgr.cpp - Surface (Elevation) package
//
// Author:  Tim Stark
// Date:    Oct 14, 2023

#include "main/core.h"
#include "universe/elevmgr.h"
#include "client.h"
#include "scene.h"
#include "surface.h"

void SurfaceTile::fixLongtitudeBoundary(SurfaceTile *nbr)
{
    if (mesh == nullptr)
        return;
}

void SurfaceTile::fixLatitudeBoundary(SurfaceTile *nbr)
{
    if (mesh == nullptr)
        return;
}

void SurfaceTile::fixCorner(SurfaceTile *nbr)
{
    if (mesh == nullptr)
        return;
    
    int16_t *elev = getElevationData();
    if (elev == nullptr)
        return;
    
    if (nbr == nullptr)
        return;
    int16_t *nelev = nbr->getElevationData();
    if (nelev == nullptr)
        return;
    
    
}

void SurfaceTile::matchEdges()
{

    SurfaceTile *lng = mgr.findTile(lod, ilng + (ilng & 1) ? 1 : -1, ilat);
    SurfaceTile *lat = mgr.findTile(lod, ilng, ilat + (ilat & 1) ? 1 : -1);
    SurfaceTile *dia = mgr.findTile(lod, ilng + (ilng & 1) ? 1 : -1, (ilat & 1) ? 1 : -1);

    if (lng != nullptr && (lng->type & TILE_VALID) == 0)
        lng = nullptr;
    if (lat != nullptr && (lat->type & TILE_VALID) == 0)
        lat = nullptr;
    if (dia != nullptr && (dia->type & TILE_VALID) == 0)
        dia = nullptr;

}

double SurfaceTile::getMeanElevation(const int16_t *elev) const
{
    int grids = mgr.getElevGrid();
    double melev = 0.0;
    for (int y = 0; y <= grids; y++, elev += ELEV_STRIDE)
        for (int x = 0; x <= grids; x++)
            melev += elev[x];
    return melev / ((grids+1)*(grids+1));
}


// int16_t *SurfaceTile::readElevationFile(int lod, int ilat, int ilng, double scale)
// {
//     elevHeader *hdr = nullptr;
//     const int nelev = ELEV_LENGTH;
//     uint8_t *ptr, *elevData = nullptr;
//     int16_t *elev = nullptr;
//     int szData = 0;

//     if (mgr.zTrees[2] != nullptr)
//     {
//         szData = mgr.zTrees[2]->read(lod+4, ilat, ilng, &elevData);
//         // logger->info("Read {} bytes from elevation database\n", szData);
//         if (szData > 0 && elevData != nullptr)
//         {
//             hdr = (elevHeader *)elevData;

//             if (hdr->code != FOURCC('E', 'L', 'E', 1))
//             {
//                 glLogger->info("*** Invalid elevation header - aborted.\n");
//                 delete [] elevData;
//                 return nullptr;
//             }

//             elev = new int16_t[nelev];
//             ptr = elevData + hdr->hdrSize;

//             switch (hdr->format)
//             {
//             case 0: // flat land (null data)
//                 for (int idx = 0; idx < nelev; idx++)
//                     elev[idx] = 0;
//                 break;

//             case 8: // unsigned byte (8-bit)
//                 for (int idx = 0; idx < nelev; idx++)
//                     elev[idx] = *ptr++;
//                 break;

//             case -16: // signed short (16-bit)
//                 int16_t *ptr16 = (int16_t *)ptr;
//                 for (int idx = 0; idx < nelev; idx++)
//                     elev[idx] = *ptr16++;
//                 break;
//             }
//         }
//     }

//     // Adjust elevation data by scale and offset
//     if (elev != nullptr)
//     {
//         // Elevation scale
//         if (hdr->scale != 0)
//         {
//             double scale = hdr->scale / scale;
//             for (int idx = 0; idx < nelev; idx++)
//                 elev[idx] = int16_t(elev[idx] * scale);
//         }

//         // Elevation offset
//         if (hdr->offset != 0)
//         {
//             int16_t ofs = int16_t(hdr->offset / scale);
//             for (int idx = 0; idx < nelev; idx++)
//                 elev[idx] += ofs;
//         }
//     }

//     // All done, release elevation data from file
//     if (elevData != nullptr)
//         delete [] elevData;
//     return elev;
// }

void SurfaceTile::interpolateElevationGrid(int ilat, int ilng, int lod,
    int pilat, int pilng, int plod, int16_t *pelev, int16_t *elev)
{
    double lat, lng, e;

    int nlat = 1 << lod;
    int nlng = 2 << lod;
    double minlat = (pi/2) * (double)(nlat-2*ilat-2)/(double)nlat;
    double maxlat = (pi/2) * (double)(nlat-2*ilat)/(double)nlat;
    double minlng = pi * (double)(2*ilng-nlng)/(double)nlng;
    double maxlng = pi * (double)(2*ilng-nlng+2)/(double)nlng;

    int pnlat = 1 << plod;
    int pnlng = 2 << plod;
    double pminlat = (pi/2) * (double)(pnlat-2*pilat-2)/(double)pnlat;
    double pmaxlat = (pi/2) * (double)(pnlat-2*pilat)/(double)pnlat;
    double pminlng = pi * (double)(2*pilng-pnlng)/(double)pnlng;
    double pmaxlng = pi * (double)(2*pilng-pnlng+2)/(double)pnlng;

    // double dlat = (maxlat-minlat)/elevGrid;
    // double dlng = (maxlng-minlng)/elevGrid;

    int16_t *elevBase = elev + ELEV_STRIDE+1;
    int16_t *pelevBase = pelev + ELEV_STRIDE+1;

    // for (int yidx = -1; yidx <= elevGrid+1; yidx++)
    // {
    //     lat = minlat + yidx*dlat;
    //     double idxlat = (lat - pminlat) * elevGrid/(pmaxlat-pminlat);
    //     int lat0 = (int)floor(idxlat);
    //     for (int xidx = -1; xidx <= elevGrid+1, xidx++)
    //     {
    //         lng = minlng + xidx*dlng;
    //         double idxlng = (lng - pminlng) * elevGrid / (pmaxlng-pminlng);
    //         int lng0 = (int)floor(idxlng);

    //         int16_t *eptr = pelevBase + lat0*elevStribe + lng0;

    //         double wlat = idxlat-lat0;
    //         double wlng = idxlng-lng0;
    //         e = (1.0 - wlat) * (eptr[0]*(1.0-wlng) + eptr[1]*wlng + wlat*(eptr[elevStride)*(1.0-wlng) + eptr[elevStride+1]*wlng);


    //         elevBase[xidx*elevStride+yidx] = (int16_t)e;

    //     }
    // }
}

bool SurfaceTile::loadElevationData()
{
    if (elev != nullptr)
        return true;

    // logger->info("Loading elevation LOD {} data\n", lod);

    elevOwn = false;
    // elev = readElevationFile(lod, ilat, ilng, mgr.elevScale);
    elev = mgr.emgr->readElevationFile(lod+4, ilat, ilng, mgr.elevScale);
    if (elev != nullptr)
    {
        // logger->info("Loaded succesfully\n", lod);
        elevOwn = true;
    }
    else if (lod > 0)
    {
        int ndat = ELEV_STRIDE*ELEV_STRIDE;
        int plod = lod-1;
        int pilat = ilat >> 1;
        int pilng = ilng >> 1;

        int16_t *pelev = nullptr;
        SurfaceTile *ptile = getParent();

        for (; plod >= 0; plod--)
        {
            if (ptile && ptile->elevOwn)
            {
                pelev = ptile->elev;
                break;
            }
            ptile = ptile->getParent();
            pilat >>= 1;
            pilng >>= 1;
        }
        if (pelev == nullptr)
            return false;

        glLogger->info("Interpolating elevation LOD {} data for LOD {}\n",
            plod, lod);

        elev = new int16_t[ndat];
        for (int idx = 0; idx < ndat; idx++)
            elev[idx] = 0;
        interpolateElevationGrid(ilat, ilng, lod, pilat, pilng, plod, pelev, elev);
    }

    return (elev != nullptr);
}

int16_t *SurfaceTile::getElevationData()
{
    if (ggelev != nullptr)
        return ggelev;

    int alod = 3; // ancestor LOD level
    while ((1 << (8-alod)) < mgr.elevGrids)
        alod--;

    if (lod >= alod)
    {
        // Quadtree patch tiles
        SurfaceTile *ptile = this;
        int blockRes = TILE_RES;
        while (blockRes > mgr.elevGrids && ptile)
        {
            ptile = ptile->getParent();
            blockRes >>= 1;
        }
        // logger->info("Elevation: LOD {} => GG LOD {}\n", lod, ptile->lod);
        if (ptile != nullptr && ptile->loadElevationData())
        {
            int mask = (TILE_RES/blockRes) - 1;
            int ofs = (((mask - ilat) & mask) * ELEV_STRIDE + (ilng & mask)) * blockRes;
            ggelev = ptile->elev + ofs;
        }

    }
    else
    {
        // Globe tiles
        // Do that later
    }

    if (ggelev != nullptr)
        elevMean = getMeanElevation(ggelev);
    return ggelev;   
}
