// surface.cpp - Surface/terrain Package
//
// Author:  Tim Stark
// Date:    Apr 17, 20222

#include "main/core.h"
#include "osd/gl/mesh.h"
#include "engine/object.h"
#include "render/scene.h"
#include "render/surface.h"

// ******** SurfaceTile ********

static tcrd_t fullRange = { 0.0, 1.0, 0.0, 1.0 };

SurfaceTile::SurfaceTile(SurfaceManager &mgr, uint32_t lod, uint32_t ilat, uint32_t ilng,
    SurfaceTile *parent)
: Tree(parent), smgr(mgr), lod(lod), ilat(ilat), ilng(ilng), tcRange(fullRange)
{
    mesh = smgr.createSphere(lod, ilat, ilng, 32, tcRange);
}

SurfaceTile::~SurfaceTile()
{
    if (mesh != nullptr)
        delete mesh;
    // if (txOwn == true && txImage != nullptr)
    //     delete txImage;
}

void SurfaceTile::render()
{
    if (mesh != nullptr)
        mesh->render();
}

// ******** SurfaceManager ********

SurfaceManager::SurfaceManager(Context &ctx, const Object &object)
: ctx(ctx), object(object)
{

    // Initialize root of virtual surface tiles
    for(int idx = 0; idx < 2; idx++)
    {
        tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
        // tiles[idx]->load();
    }
}

SurfaceManager::~SurfaceManager()
{
    for (int idx = 0; idx < 2; idx++)
        delete tiles[idx];
}

void SurfaceManager::render(SurfaceTile *tile)
{
    tile->render();
}

void SurfaceManager::render(renderParam &prm, ObjectProperties &op)
{
    for (int idx = 0; idx < 2; idx++)
        render(tiles[idx]);
}

Mesh *SurfaceManager::createSphere(int lod, int ilat, int ilng, int grids, const tcrd_t &tcr)
{
	int nlat = 1 << lod;
	int nlng = 2 << lod;

	double mlat0 = pi * double(ilat) / double(nlat);
	double mlat1 = pi * double(ilat+1) / double(nlat);
    // float mlng0 = PI*2 * (float(nlng/2 - ilng-1) / float(nlng)) - PI;
    // float mlng1 = PI*2 * (float(nlng/2 - ilng) / float(nlng)) - PI;
    double mlng0 = pi*2.0 * (double(ilng) / double(nlng)) - pi;
    double mlng1 = pi*2.0 * (double(ilng+1) / double(nlng)) - pi;

    // cout << "ilng " << ilng << " nlng " << nlng << " mlng0 " << mlng0 << " mlng1 " << mlng1 << 
    //     " ( " << toDegree(mlng0) << " , " << toDegree(mlng1) << " )" << endl;

	double   rad  = object.getRadius();
	double   erad = rad;

    double slng, clng;
    double slat, clat;
	double lng, lat;
    double tu, tv, du, dv;
    double tur, tvr;
    vec3d_t pos, nml;

    int      vidx;
    int      nVertices;
    int      nIndices, nIndices1;

    vtxef_t  *vtx;
    uint16_t *idx, *pidx;

    nVertices = (grids+1)*(grids+1);
    nIndices  = 6 * (grids*grids);
    vtx       = new vtxef_t[nVertices];
    idx       = new uint16_t[nIndices];

//    int      nTexCoords;
//    double *vtxs, *nmls;
//	double *pvtx, *pnml;
//    double   *tc, *ptc;
//    nVertices   = 3 * ((grids+1)*(grids+1));
//    nTexCoords  = 2 * ((grids+1)*(grids+1));
//    vtxs  = new double[nVertices];
//    nmls  = new double[nVertices];
//    tc    = new double[nTexCoords];
//    pvtx  = vtxs;
//    pnml  = nmls;
//    ptc   = tc;

//	std::cout << std::fixed << std::setprecision(10);
//	std::cout << "----------------------" << std::endl;
//	std::cout << "LOD:  " << lod << " nLAT: " << nlat << " nLNG: " << nlng
//			  << " Grids: " << grids << std::endl;
//	std::cout << "iLAT: " << ilat << " iLNG: " << ilng << std::endl;
//	std::cout << "Latitude Range:  " << toDegrees(mlat0) << " to " << toDegrees(mlat1) << std::endl;
//	std::cout << "Longitude Range: " << toDegrees(mlng0) << " to " << toDegrees(mlng1) << std::endl;

//	std::cout << "Delta Angle: " << dang << " U:" << du << std::endl;
//	std::cout << "Alpha: " << alpha << std::endl;

    du  = (mlng1 - mlng0) / grids;
    dv  = (mlat1 - mlat0) / grids;
    tur = tcr.tumax - tcr.tumin;
    tvr = tcr.tvmax - tcr.tvmin;

    vidx = 0;
	for (int y = 0; y <= grids; y++)
	{
		lat  = mlat0 + (mlat1-mlat0) * (double(y)/double(grids));
		slat = sin(lat); clat = cos(lat);
        tv = tcr.tvmin + tvr * (double(y)/double(grids));

//        std::cout << "Y = " << y << " LAT: " << toDegrees(lat) << std::endl;

		for (int x = 0; x <= grids; x++)
		{
			lng  = mlng0 + (mlng1-mlng0) * (double(x)/double(grids));
			slng = sin(lng); clng = cos(lng);
            tu   = tcr.tumin + tur * (double(x)/double(grids));

//            std::cout << "X = " << x << " LNG: " << toDegrees(lng) << std::endl;

//            pos = vec3d_t(axes.x()*slat*clng, axes.y()*clat, axes.z()*slat*slng);
//            nml = pos.normalized();

//            erad = rad + elevGlobe;
//            if (elev != nullptr) {
//            	int16_t edata = elev[(y+1)*ELEV_STRIDE + (x+1)];
//            	erad += (double(edata) * elevScale) / 1000.0;
//            	if (edata != 0)
//            		std::cout << "Elev X: " << x << " Y: " << y
//						      << "Elev: " << edata << std::endl;
//            }
//            else
//            	std::cout << "No elevation data for sphere..." << std::endl;

            nml = vec3d_t(slat*clng, clat, slat*-slng);

            pos = nml * erad;

			// Convert to 32-bit floats for vertices buffer/rendering
            vtx[vidx].vx = float(pos.x);
            vtx[vidx].vy = float(pos.y);
            vtx[vidx].vz = float(pos.z);

       		vtx[vidx].ex = float(pos.x - vtx[vidx].vx);
            vtx[vidx].ey = float(pos.y - vtx[vidx].vy);
            vtx[vidx].ez = float(pos.z - vtx[vidx].vz);

            vtx[vidx].nx = float(nml.x);
            vtx[vidx].ny = float(nml.y);
            vtx[vidx].nz = float(nml.z);

            vtx[vidx].tu = float(tu);
            vtx[vidx].tv = float(tv);

//            if (vidx == 0) {
//            	cout << "Value:   " << setw(15) << fixed << pos.x << "," << pos.y << "," << pos.z << endl;
//            	cout << " Vertex: " << setw(15) << fixed << vtx[vidx].vx << "," << vtx[vidx].vy << "," << vtx[vidx].vz << endl;
//            	cout << " Error:  " << setw(15) << fixed << vtx[vidx].ex << "," << vtx[vidx].ey << "," << vtx[vidx].ez << endl;
//            }

            vidx++;

//			std::cout << "(" << x << "," << y << "): " << lng0 << " " << lat0 << " --> ";
//			std::cout << "(" << nml0.x() << "," << nml0.y() << "," << nml0.z() << ")" << std::endl;
		}
//		std::cout << std::endl;
//		std::cout << "Actual: " << pvtx - vtxs << " Total: " << nvtx << std::endl;
//		std::cout << "Estimate: " << (grids+1)*2*(grids-1)+2 << std::endl;

		// Degenerate triangles to connect
//		pvtx[0] = pvtx[3] = pvtx[-3];
//		pvtx[1] = pvtx[4] = pvtx[-2];
//		pvtx[2] = pvtx[5] = pvtx[-1];
//
//		pnml[0] = pnml[3] = pnml[-3];
//		pnml[1] = pnml[4] = pnml[-2];
//		pnml[2] = pnml[5] = pnml[-1];

	}
//	std::cout << "Vertices: Actual: " << vidx << " Expect: " << nVertices1 << std::endl;

    nIndices1 = 0;
    pidx = idx;
    for (int y = 0; y < grids; y++) {
        for (int x = 0; x < grids; x++) {
            *pidx++ = (y+0)*(grids+1) + (x+0);
            *pidx++ = (y+1)*(grids+1) + (x+0);
            *pidx++ = (y+0)*(grids+1) + (x+1);

            *pidx++ = (y+1)*(grids+1) + (x+0);
            *pidx++ = (y+1)*(grids+1) + (x+1);
            *pidx++ = (y+0)*(grids+1) + (x+1);

//            std::cout << "Index: (" << pidx[0] << "," << pidx[1] << "," << pidx[2] << ")" << std::endl;
//            std::cout << "Index: (" << pidx[3] << "," << pidx[4] << "," << pidx[5] << ")" << std::endl;
//
//            pidx += 6;
            nIndices1 += 6;
        }
    }

    return Mesh::create(ctx, vidx, vtx, nIndices, idx);
}
