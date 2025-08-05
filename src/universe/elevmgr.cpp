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
    // zTrees[0] = zTreeManager::create(folder, "surf");
    // zTrees[1] = zTreeManager::create(folder, "mask"); 
    zTrees[0] = zTreeManager::create(folder, "elev");
    zTrees[1] = zTreeManager::create(folder, "elev_mod");
    // zTrees[4] = zTreeManager::create(folder, "label");
}

int16_t *ElevationManager::readElevationFile(int lod, int ilat, int ilng, double elevScale) const
{
    elevHeader *hdr = nullptr;
    const int nelev = ELEV_LENGTH;
    uint8_t *ptr, *elevData = nullptr;
    int16_t *elev = nullptr;
    int szData = 0;
    double rescale;
    int16_t offset;

    int nlat = 1 << lod;
    int nlng = 2 << lod;

    if (zTrees[0] != nullptr)
    {
        szData = zTrees[0]->read(lod, ilat, ilng, &elevData);
        // ofsLogger->info("Read {} bytes from elevation database\n", szData);
        if (szData > 0 && elevData != nullptr)
        {
            hdr = (elevHeader *)elevData;

            if (hdr->code != FOURCC('E', 'L', 'E', 1))
            {
                ofsLogger->info("*** Invalid elevation header - aborted.\n");
                delete [] elevData;
                return nullptr;
            }

            elev = new int16_t[nelev];
            ptr = elevData + hdr->hdrSize;
            rescale = (hdr->scale != elevScale) ? hdr->scale / elevScale : 1.0;
            offset = (hdr->offset != 0.0) ? (int16_t)(hdr->offset / elevScale) : 0;

            switch (hdr->format)
            {
            case 0: // flat land (null data)
                for (int idx = 0; idx < nelev; idx++)
                    elev[idx] = offset;
                break;

            case 8: // unsigned byte (8-bit)
                {
                    uint8_t *ptr8 = (uint8_t *)ptr;
                    for (int idx = 0; idx < nelev; idx++,ptr8++)
                        elev[idx] = (*ptr8 * rescale) + offset;
                }
                break;

            case -16: // signed short (16-bit)
                {
                    int16_t *ptr16 = (int16_t *)ptr;
                    for (int idx = 0; idx < nelev; idx++,ptr16++)
                        elev[idx] = (*ptr16 * rescale) + offset;
                }
                break;
            }

            // All done, release elevation data from file
            delete [] elevData;

            // uint32_t cksum = 0;
            // for (int idx = 0; idx < ELEV_LENGTH; idx++)
            //     cksum += elev[idx];
            // ofsLogger->debug("Elevation Scale: {:f} Offset: {:f}\n", hdr->scale, hdr->offset);
            // ofsLogger->debug("Data: {:p} Checksum: {:x}\n", fmt::ptr(elev), cksum);
        }
    }

    return elev;
}

bool ElevationManager::readElevationModFile(int lod, int ilat, int ilng, double elevScale, int16_t *elev) const
{
    elevHeader *hdr = nullptr;
    const int nelev = ELEV_LENGTH;
    uint8_t *ptr, *elevData = nullptr;
    int szData = 0;
    double rescale;
    int16_t offset;

    int nlat = 1 << lod;
    int nlng = 2 << lod;

    if (zTrees[1] != nullptr)
    {
        szData = zTrees[1]->read(lod, ilat, ilng, &elevData);
        // ofsLogger->info("Read {} bytes from elevation (modified) database\n", szData);
        if (szData > 0 && elevData != nullptr)
        {
            hdr = (elevHeader *)elevData;

            if (hdr->code != FOURCC('E', 'L', 'E', 1))
            {
                ofsLogger->info("*** Invalid elevation (modified) header - aborted.\n");
                delete [] elevData;
                return false;
            }

            // elev = new int16_t[nelev];
            ptr = elevData + hdr->hdrSize;

            rescale = (hdr->scale != elevScale) ? hdr->scale / elevScale : 1.0;
            offset = (hdr->offset != 0.0) ? (int16_t)(hdr->offset / elevScale) : 0;

            switch (hdr->format)
            {
            case 0: // flat land (null data)
                for (int idx = 0; idx < nelev; idx++)
                    elev[idx] = offset;
                break;

            case 8: // unsigned byte (8-bit)
                {
                    uint8_t mask8 = UCHAR_MAX;
                    uint8_t *ptr8 = (uint8_t *)ptr;
                    for (int idx = 0; idx < nelev; idx++,ptr8++)
                        if (*ptr8 != mask8)
                            elev[idx] = (*ptr8 * rescale) + offset;
                }
                break;

            case -16: // signed short (16-bit)
                {
                    uint16_t mask16 = SHRT_MAX;
                    int16_t *ptr16 = (int16_t *)ptr;
                    for (int idx = 0; idx < nelev; idx++,ptr16++)
                        if (*ptr16 != mask16)
                            elev[idx] = (*ptr16 * rescale) + offset;
                }
                break;
            }

            // All done, release elevation data from file
            delete [] elevData;
        }

        return true;
    }

    return false;
}

// int16_t *ElevationManager::getElevationData()
// {
//     return nullptr;
// }

bool ElevationManager::getTileIndex(double lat, double lng, int lod, int &ilat, int &ilng) const
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    ilat = (int)((pi05 - lat) / pi * nlat);
    ilng = (int)((lng + pi) / pi2 * nlng);

    // ofsLogger->debug("Location: {:f}, {:f}\n", glm::degrees(lat), glm::degrees(lng));
    // ofsLogger->debug("Tile Index: {}/{}, {}/{} LOD: {}\n", ilat, nlat, ilng, nlng, lod);

    return true;
}

double ElevationManager::getElevationData(glm::dvec3 loc, int reqlod,
    elevTileList_t *elevTiles, glm::dvec3 *normal, int *lod) const
{
    double e = 0.0;

    if (zTrees[0] == nullptr)
        return 0.0;

    if (elevMode > 0)
    {
        elevTileList_t &tiles = (elevTiles != nullptr) ? *elevTiles : localTiles;
        ElevationTile *t = nullptr;
        int lod, ilat, ilng;

        for (auto tile : tiles)
        {
            if (tile.data != nullptr && reqlod == tile.tgtlod &&
                loc.x >= tile.latmin && loc.x <= tile.latmax &&
                loc.y >= tile.lngmin && loc.y <= tile.lngmax)
            {
                t = &tile;
                break;
            }
        }

        if (t == nullptr)
        {
            // Find oldest elevation tile
            t = &tiles[0];
            for (int idx = 1; idx < elevTiles->size(); idx++)
                if (tiles[idx].lastAccess < t->lastAccess)
                    t = &tiles[idx];

            // Release old elevation data
            if (t->data != nullptr)
            {
                delete [] t->data;
                t->data = nullptr;
            }

            for (lod = reqlod; lod >= 0; lod--)
            {
                getTileIndex(loc.x, loc.y, lod, ilat, ilng);
                t->data = readElevationFile(lod+4, ilat, ilng, elevScale);

                if (t->data != nullptr)
                {
                    readElevationModFile(lod+4, ilat, ilng, elevScale, t->data);

                    int nlat = 1 << lod;
                    int nlng = 2 << lod;

                    t->lod  = lod;
                    t->ilat = ilat, t->nlat = nlat;
                    t->ilng = ilng, t->nlng = nlng;
                    t->tgtlod = reqlod;
                    t->latmin = (0.5 - (double(ilat+1)/double(nlat)))*pi;
                    t->latmax = (0.5 - (double(ilat)/double(nlat)))*pi;
                    t->lngmin = double(ilng)/double(nlng)*pi2 - pi;
                    t->lngmax = double(ilng+1)/double(nlng)*pi2 - pi;

                    // ofsLogger->debug("LOD Level:    {}\n", lod);
                    // ofsLogger->debug("Index:        {}/{}, {}/{}\n", ilat, nlat, ilng, nlng);
                    // ofsLogger->debug("Lat  Min/Max: {:f} - {:f}\n", glm::degrees(t->latmin), glm::degrees(t->latmax));
                    // ofsLogger->debug("Long Min/Max: {:f} - {:f}\n", glm::degrees(t->lngmin), glm::degrees(t->lngmax));
                    // ofsLogger->debug("Location:     {:f}, {:f}\n", glm::degrees(loc.x), glm::degrees(loc.y));

                    // uint32_t cksum = 0;
                    // for (int idx = 0; idx < ELEV_LENGTH; idx++)
                    //     cksum += t->data[idx];
                    // ofsLogger->debug("Data: {:p} Checksum: {:x}\n", fmt::ptr(t->data), cksum);

                    break;
                }
            }

            t->lat0 = t->lng0 = 0;
        }

        if (t->data != nullptr)
        {
            int16_t *elevBase = t->data + ELEV_STRIDE + 1;
            double latidx = (loc.x - t->latmin) * elevGrid / (t->latmax - t->latmin);
            double lngidx = (loc.y - t->lngmin) * elevGrid / (t->lngmax - t->lngmin);
            int lat0 = (int)latidx;
            int lng0 = (int)lngidx;

            // ofsLogger->debug("Tile Index: {}, {}\n", lat0, lng0);
            // dump(t->data);

            int16_t *eptr = elevBase + lat0 * ELEV_STRIDE + lng0;
            if (elevMode == 1)
            {
                double wlat = latidx - lat0;
                double wlng = lngidx - lng0;

                // Determine elevation
                double e1 = eptr[0]*(1.0-wlng) + eptr[1]*wlng;
                double e2 = eptr[ELEV_STRIDE]*(1.0-wlng) + eptr[ELEV_STRIDE+1]*wlng;
                e = e1*(1.0-wlat) + e2*wlat;

                // ofsLogger->debug("Tile LOD: {} Index ({},{})\n", t->lod, t->ilat, t->ilng);
                // ofsLogger->debug("Loc: {:f} {:f} ==> Elev {:f}\n", wlat, wlng, e);
                // ofsLogger->debug("Data: {} {}\n", eptr[0], eptr[1]);
                // ofsLogger->debug("Data: {} {}\n", eptr[ELEV_STRIDE], eptr[ELEV_STRIDE+1]);
                
                // uint32_t cksum = 0;
                // for (int idx = 0; idx < ELEV_LENGTH; idx++)
                //     cksum += t->data[idx];
                // ofsLogger->debug("Data: {:p} Checksum: {:x}\n", fmt::ptr(t->data), cksum);

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
