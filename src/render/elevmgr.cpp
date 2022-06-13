// elevation.cpp - Elevation data package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "osd/gl/mesh.h"
#include "render/scene.h"
#include "render/surface.h"
#include "render/elevmgr.h"

int16_t *SurfaceTile::readElevationFile(int lod, int ilat, int ilng, double eres)
{
    elevHeader *hdr = nullptr;
    const int nelev = ELEV_LENGTH;
    uint8_t *prt, *elevData = nullptr;
    int16_t *elev = nullptr;


//    if (mgr->tmgr[2] != nullptr) {
//        res = mgr->tmgr[2]->read(lod+4, ilat, ilng, &elevData);
//        if (res > 0 && elevData != nullptr) {
//            hdr = (elevHeader *)elevData;
//
//            if (hdr->code != FOURCC('E', 'L', 'E', 1)) {
//                cerr << "*** Invalid elevation header - aborted." << endl;
//                delete []elevData;
//                return nullptr;
//            }
//
//            elev = new int16_t[nelev];
//            ptr = elevData + hdr->hdrSize;
//
//            cout << "Loading elevation data at LOD " << lod+4 << endl;
//            cout << "Format: " << hdr->format << " Header size: " << hdr->hdrSize << endl;
//            cout << "Grid X: " << hdr->xgrd << " Y: " << hdr->ygrd << endl;
//            cout << "Pad  X: " << hdr->xpad << " Y: " << hdr->ypad << endl;
//            cout << "Scale: " << hdr->scale << " Offset: " << hdr->offset << endl;
//            cout << "Latitude:  Min " << hdr->latmin << " Max: " << hdr->latmax << endl;
//            cout << "Longitude: Min " << hdr->lngmin << " Max: " << hdr->lngmax << endl;
//            cout << "Mean:      Min " << hdr->emin << " Max: " << hdr->emax << " Mean: " << hdr->emean << std::endl;
//
////            cout << "HDR Off Scale: " << int64_t(&hdr->scale) - int64_t(hdr) << " Offset: " << int64_t(&hdr->offset) - int64_t(hdr) << endl;
//
//            switch (hdr->format) {
//            case 0: // flat (null data)
//                for (int idx = 0; idx < nelev; idx++)
//                    elev[idx] = 0;
//                break;
//
//            case 8: // unsigned byte (8-bit)
//                for (int idx = 0; idx < nelev; idx++)
//                    elev[idx] = uint16_t(*ptr++);
//                break;
//
//            case -16: // signed short (16-bit)
//            	int16_t *ptr16 = (int16_t *)ptr;
//            	for (int idx = 0; idx < nelev; idx++) {
//            		elev[idx] = *ptr16++;
////            		std::cout << "Elev data: " << elev[idx] << std::endl;
//            	}
//                break;
//            }
//            delete []elevData;
//        }
//    }

    // Validate elevation header and set up new elevation data
//    if (elevData != nullptr) {
//        hdr = (elevHeader *)elevData;
//    	if (hdr->code != FOURCC('E', 'L', 'E', 1)) {
//            std::cerr << "*** Invalid elevation header - aborted." << std::endl;
//            delete []elevData;
//            return nullptr;
//    	}
//
//        elev = new int16_t[nelev];
//        ptr  = elevData + hdr->hdrSize;
//
//    	switch (hdr->format) {
//    	case 0: // flat (null data)
//    		for (int idx = 0; idx < nelev; idx++)
//    			elev[idx] = 0;
//    		break;
//
//    	case 8: // unsigned byte (8-bit)
//    		for (int idx = 0; idx < nelev; idx++)
//    			elev[idx] = int16_t(*ptr++);
//    		break;
//
//    	case -16: // signed short (16-bit)
//    		std::copy(ptr, ptr + nelev*sizeof(int16_t), elev);
//    		break;
//    	}
//
//    	delete []elevData;
//    }

    // Adjust elevation data by scale and offset
//    if (elev != nullptr) {
//        // Elevation scale
//        if (hdr->scale != 0) {
//            double scale = hdr->scale / eres;
//            for (int idx = 0; idx < nelev; idx++)
//                elev[idx] = int16_t(elev[idx] * scale);
//        }
//        // Elevation offset
//        if (hdr->offset != 0) {
//            int16_t ofs = int16_t(hdr->offset / eres);
//            for (int idx = 0; idx < nelev; idx++)
//                elev[idx] += ofs;
//        }
//        cout << "Loaded elevation data at LOD " << lod+4 << endl;
//    } else
//    	cout << "Elevation data is not available at LOD " << lod+4 << endl;

    return elev;
}

bool SurfaceTile::interpolateLinearElevationGrid(const float *inElev,  float *outElev)
{
    int q0 = 0, c = 129;

    if (!(ilat & 1))
        q0 = TILE_ELEVRES * 128;
    if (ilng & 1)
        q0 += 128;

    for (int i = 0; i <= c; i++)
    {
        int q1 = q0 + 1;
        int q2 = q0 + TILE_ELEVRES;
        int q3 = q1 + TILE_ELEVRES;
        int x = (TILE_ELEVRES << 1) * i;

        for (int k = 0; k <= c; k++)
        {
            float f0 = inElev[k + q0], f1 = inElev[k + q1];
            float f2 = inElev[k + q2], f3 = inElev[k + q3];

            outElev[x + 0] = (f0 + f1 + f2 + f3) / 4.0;
            if (k != c)
                outElev[x + 1] = (f1 + f3) / 2.0;

            if (i != c)
            {
                outElev[x + TILE_ELEVRES] = (f2 + f3) / 2.0;
                if (k != c)
                    outElev[x + TILE_ELEVRES + 1] = f3;
            }
            x += 2;
        }
        q0 += TILE_ELEVRES;
    }

    return true;
}

bool SurfaceTile::interpolateCubicElevationGrid(int16_t *elev, int lod, int ilat, int ilng,
    int16_t *pElev, int plod, int pilat, int pilng,
    double *eMean)
{
    double elevGrid = 32;
    
    const int nlat = 1 << lod;
    const int nlng = 2 << lod;

    double mlat0 = (pi / 2.0) * (double(nlat-2 * ilat-2) / double(nlat));
    double mlat1 = (pi / 2.0) * (double(nlat-2 * ilat) / double(nlat));
    double mlng0 = pi * (double(ilng*2 - nlng) / double(nlng));
    double mlng1 = pi * (double(ilng*2 - nlng+2) / double(nlng));
    double dlat  = (mlat1 - mlat0) / elevGrid;
    double dlng  = (mlng1 - mlng0) / elevGrid;

    const int pnlat = 1 << plod;
    const int pnlng = 2 << plod;

    double pmlat0 = (pi / 2.0) * (double(pnlat-2 * pilat-2) / double(pnlat));
    double pmlat1 = (pi / 2.0) * (double(pnlat-2 * pilat) / double(pnlat));
    double pmlng0 = pi * (double(pilng*2 - pnlng) / double(pnlng));
    double pmlng1 = pi * (double(pilng*2 - pnlng+2) / double(pnlng));
    double pdlat  = (pmlat1 - pmlat0) / elevGrid;
    double pdlng  = (pmlng1 - pmlng0) / elevGrid;

    return false;
}

bool SurfaceTile::loadElevationData()
{
    if (elev != nullptr)
        return true;

    return false;
}

// bool TerrainTile::loadElevation()
// {
// 	cout << "Elevation Tile LOD: " << lod+4 << " Lat: " << ilat << " Lng: " << ilng << endl;

//     // Check if elevation data was already loaded from database
//     if (elev != nullptr) {
//     	cout << "Already have elevation data - no action..." << endl;
//     	return true;
//     }

//     elevOwn = false;
//     elev = readElevation(lod, ilat, ilng, 1.0);
//     if (elev != nullptr)
//     	elevOwn = true;
//     else if (lod > 0) {
//     	cout << "Interpolating elevation data" << endl;

//     	int plod = lod - 1;
//     	int plng = ilng >> 1;
//     	int plat = ilat >> 1;
//     	int16_t *pelev = nullptr;
//     	TerrainTile *ptile = getParent();

//     	// Find ancestor tile with elevation data
//     	for (; plod >= 0; plod--) {
//     		if (ptile != nullptr && ptile->hasElevData()) {
//     			pelev = ptile->getElevData();
//     			break;
//     		}
//     		ptile = ptile->getParent();
//     		plng >>= 1;
//     		plat >>= 1;
//     	}
//     	if (pelev == nullptr)
//     		return false;

//     	// Interpolate ancestor data for that LOD level.
// //    	elev = new int16_t[TILE_NELEV];
// //    	mgr->interpolateElevData(plod, plng, plat, lod, lng, lat, pelev, elev);
//     }
//     return elev != nullptr;
// }

float *SurfaceTile::getElevationData()
{
    // If great-grandfather eleveation data
    // already is loaded, just return it.
    if (ggelev != nullptr)
        return ggelev;

    int gglod = 3;
    while ((1u << (8 - gglod)) < smgr.gridRes)
        gglod--;

    SurfaceTile *ggTile = nullptr;
    int resBlock = TILE_RES;
    if (lod >= gglod)
    {
        ggTile = this;

        // Find parent tile (LOD level 3+)
        while (resBlock > smgr.gridRes && ggTile != nullptr)
        {
            ggTile = ggTile->getParent();
            resBlock >>= 1;
        }
        if (ggTile != nullptr && ggTile->loadElevationData())
        {
            int nBlock = TILE_FILERES / resBlock;
            int mask = nBlock - 1;
            int ofs = ((mask - ilat & mask) * TILE_ELEVRES + (ilng & mask)) * resBlock;
            ggelev = ggTile->elev + ofs;
        }
    }
    else
    {
        // Find global tile (LOD level 0-2)

    }

    return ggelev;
}
