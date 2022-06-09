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

// int16_t *TerrainTile::getElevationData()
// {
// 	TerrainTile *ptile = nullptr;
// 	int      resBlock = ELEV_RES;
// 	int      ofs;
// 	uint32_t mask;
// 	int      glod = 3;

// 	// If grand-grandfather elevation data
// 	// already is loaded, just return it.
// 	if (ggelev != nullptr)
// 		return ggelev;

// 	while ((1u << (8 - glod)) < tmgr.resGrid)
// 		glod--;

// //	cout << "Current LOD: " << lod+3 << " Elev LOD: " << glod << endl;

// 	if (lod >= glod) {
// 		// Find parent tile (LOD level 3+)
// 		ptile = this;
// //		cout << "LOD: " << ptile->lod+3 << " Block Resolution: " << resBlock << endl;
// 		while (resBlock > tmgr.resGrid && ptile != nullptr) {
// 			ptile = ptile->getParent();
// 			resBlock >>= 1;
// 		}
// 	} else {
// 		// Find globe tile (LOD level 0-2)
// 	}

// 	if (ptile != nullptr) {
// //		cout << "Block Resolution: " << resBlock << " Parent Tile LOD: " << ptile->lod+3 << endl;

// //		if (ptile->loadElevation()) {
// //			mask = (ELEV_RES / resBlock) - 1;
// //			ofs  = ((mask - ilat & mask)*ELEV_STRIDE + (mask - ilng & mask)) * resBlock;
// //			cout << "Tile Lat: " << ilat << " Lng: " << ilng << endl;
// //			cout << "GG Tile Lat: " << ptile->ilat << " Lng: " << ptile->ilng
// //			     << "Elev X: " << (ilng & mask) << " Y: " << (ilat & mask) << endl;
// //			ggelev = ptile->elev + ofs;
// //			cout << "Got new elevation data ready" << endl;
// //		}
// 	}

// 	return ggelev;
// }
