// elevmgr.cpp - Elevation Manager package
//
// Author:  Tim Stark
// Date:    Oct 18, 2023

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/surfmgr.h"
#include "universe/elevmgr.h"


ElevationManager::ElevationManager(CelestialPlanet *obj)
: object(obj)
{
}

void ElevationManager::setup(const fs::path &folder)
{ 
    zTrees[0] = zTreeManager::create(folder, "elev");
    zTrees[1] = zTreeManager::create(folder, "elev_mod");
}

// int16_t *ElevationManager::loadElevationTile(int lod, int ilat, int ilng, double elevRes) const
// {
//     int16_t *elev = nullptr;

//     // if (mode > 0)
//     // {
//     //     const int ndat = elevStride*elevStride;

//     //     if (zTrees[0] != nullptr)
//     //     {
//     //         int szData = zTrees[0]->read(lod, ilat, ilng, &data);
//     //         if (data != nullptr)
//     //         {

//     //         }
//     //     }
//     // }

//     return elev;
// }

int16_t *ElevationManager::readElevationFile(int lod, int ilat, int ilng, double scale) const
{
    elevHeader *hdr = nullptr;
    const int nelev = ELEV_LENGTH;
    uint8_t *ptr, *elevData = nullptr;
    int16_t *elev = nullptr;
    int szData = 0;

    int nlat = 1 << lod;
    int nlng = 2 << lod;

    if (zTrees[0] != nullptr)
    {
        szData = zTrees[0]->read(lod+4, ilat, ilng, &elevData);
        // logger->info("Read {} bytes from elevation database\n", szData);
        if (szData > 0 && elevData != nullptr)
        {
            hdr = (elevHeader *)elevData;

            if (hdr->code != FOURCC('E', 'L', 'E', 1))
            {
                // logger->info("*** Invalid elevation header - aborted.\n");
                delete [] elevData;
                return nullptr;
            }

            elev = new int16_t[nelev];
            ptr = elevData + hdr->hdrSize;

            switch (hdr->format)
            {
            case 0: // flat land (null data)
                for (int idx = 0; idx < nelev; idx++)
                    elev[idx] = 0;
                break;

            case 8: // unsigned byte (8-bit)
                for (int idx = 0; idx < nelev; idx++)
                    elev[idx] = *ptr++;
                break;

            case -16: // signed short (16-bit)
                int16_t *ptr16 = (int16_t *)ptr;
                for (int idx = 0; idx < nelev; idx++)
                    elev[idx] = *ptr16++;
                break;
            }
        }
    }

    // Adjust elevation data by scale and offset
    if (elev != nullptr)
    {
        // Elevation scale
        if (hdr->scale != 0)
        {
            double scale = hdr->scale / scale;
            for (int idx = 0; idx < nelev; idx++)
                elev[idx] = int16_t(elev[idx] * scale);
        }

        // Elevation offset
        if (hdr->offset != 0)
        {
            int16_t ofs = int16_t(hdr->offset / scale);
            for (int idx = 0; idx < nelev; idx++)
                elev[idx] += ofs;
        }
    }

    // All done, release elevation data from file
    if (elevData != nullptr)
        delete [] elevData;
    return elev;
}

// int16_t *ElevationManager::getElevationData()
// {
//     return nullptr;
// }

bool ElevationManager::getTileIndex(double lat, double lng, int lod, int &ilat, int &ilng) const
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    ilat = (int)(((pi/2.0) - lat) / (pi * nlat));
    ilng = (int)((lng + pi) / ((pi*2.0) * nlng));

    return true;
}

double ElevationManager::getElevationData(glm::dvec3 loc, int reqlod,
    elevTileList_t *elevTiles, glm::dvec3 *normal, int *lod) const
{
    double e = 0.0;

    if (elevMode > 0)
    {
        elevTileList_t &tiles = (elevTiles != nullptr) ? *elevTiles : localTiles;
        ElevationTile *t = nullptr;
        int lod, ilat, ilng;

        for (auto tile : tiles)
        {
            if (tile->data != nullptr && reqlod == tile->tgtlod &&
                loc.x >= tile->latmin && loc.x <= tile->latmax &&
                loc.y >= tile->lngmin && loc.y <= tile->lngmax)
            {
                t = tile;
                break;
            }
        }

        if (t == nullptr)
        {
            // Find oldest elevation tile
            t = tiles[0];
            for (int idx = 1; idx < elevTiles->size(); idx++)
                if (tiles[idx]->lastAccess < t->lastAccess)
                    t = tiles[idx];

            // Release old elevation data
            if (t->data != nullptr)
            {
                delete [] t->data;
                t->data = nullptr;
            }

            for (lod = reqlod; lod >= 0; lod--)
            {
                getTileIndex(loc.x, loc.y, lod, ilat, ilng);
                t->data = readElevationFile(lod+4, ilat, ilng, elevGrids);

                if (t->data != nullptr)
                {
                    int nlat = 1 << lod;
                    int nlng = 2 << lod;

                    t->lod = lod;
                    t->tgtlod = reqlod;
                    t->latmin = (0.5 - (double(ilat+1)/double(nlat)))*pi;
                    t->latmax = (0.5 - (double(ilat)/double(nlat)))*pi;
                    t->lngmin = double(ilng)/double(nlng)*(pi*2.0) - pi;
                    t->lngmax = double(ilng+1)/double(nlng)*(pi*2.0) - pi;

                    break;
                }
            }

            t->lat0 = t->lng0 = 0;
        }

        if (t->data != nullptr)
        {
            int16_t *elevBase = t->data + ELEV_STRIDE + 1;
            double latidx = (loc.x - t->latmin) * elevGrids / (t->latmax - t->latmin);
            double lngidx = (loc.y - t->lngmin) * elevGrids / (t->lngmax - t->lngmin);
            int lat0 = (int)latidx;
            int lng0 = (int)lngidx;

            int16_t *eptr = elevBase + lat0 * ELEV_STRIDE + lng0;
            if (elevMode == 1)
            {
                double wlat = latidx - lat0;
                double wlng = lngidx - lng0;

                // Determine elevation
                double e1 = eptr[0]*(1.0-wlng) + eptr[1]*wlng;
                double e2 = eptr[ELEV_STRIDE]*(1.0-wlng) + eptr[ELEV_STRIDE+1]*wlng;
                e = e1*(1.0-wlat) + e2*wlat;

                // Determine normals
                if (normal != nullptr)
                {
                    double dlat = (t->latmax - t->latmin) / elevGrids;
                    double dlng = (t->lngmax - t->lngmin) / elevGrids;
                    double dz = dlat * object->getRadius();
                    double dx = dlng * object->getRadius() * cos(loc.x);

                    double nx1 = eptr[1]-eptr[0];
                    double nx2 = eptr[ELEV_STRIDE+1] - eptr[ELEV_STRIDE];
                    double nx = wlat*nx2 + (1.0-wlat)*nx1;
                    glm::dvec3 vnx(dx, nx, 0);

                    double nz1 = eptr[ELEV_STRIDE] - eptr[0];
                    double nz2 = eptr[ELEV_STRIDE+1] - eptr[1];
                    double nz = wlng*nz2 + (1.0-wlng)*nz1;
                    glm::dvec3 vnz(0, nz, dz);

                    *normal = glm::normalize(glm::cross(vnx, vnz));
                }
            }
            else if (elevMode == 2)
            {

            }

            // t->lastAccess = td.syst1;
            t->lat0 = lat0;
            t->lng0 = lng0;
        }
    }

    return e * elevScale;
}
