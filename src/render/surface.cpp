// surface.cpp - Surface/terrain Package
//
// Author:  Tim Stark
// Date:    Apr 17, 20222

#include "main/core.h"
#include "osd/gl/context.h"
#include "osd/gl/shader.h"
#include "osd/gl/mesh.h"
#include "osd/gl/texture.h"
#include "engine/object.h"
#include "universe/body.h"
#include "universe/star.h"
#include "render/scene.h"
#include "render/surface.h"

// ******** SurfaceTile ********

static tcrd_t fullRange = { 0.0, 1.0, 0.0, 1.0 };

SurfaceTile::SurfaceTile(SurfaceManager &mgr, uint32_t lod, uint32_t ilat, uint32_t ilng,
    SurfaceTile *parent)
: Tree(parent), smgr(mgr), lod(lod), ilat(ilat), ilng(ilng), tcRange(fullRange)
{
    center = setCenter();
    // mesh = smgr.createSphere(lod, ilat, ilng, 32, tcRange);
}

SurfaceTile::~SurfaceTile()
{
    if (mesh != nullptr)
        delete mesh;
    // if (txOwn == true && txImage != nullptr)
    //     delete txImage;
}

vec3d_t SurfaceTile::setCenter()
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    // double mlat0 = pi * double(ilat) / double(nlat);
    // double mlat1 = pi * double(ilat+1) / double(nlat);

    // double mlng0 = pi*2 * (double(ilng) / double(nlng)) - pi;
    // double mlng1 = pi*2 * (double(ilng+1) / double(nlng)) - pi;

    double latc = pi * ((double(ilat)+0.5) / double(nlat));
    double lngc = pi*2 * ((double(ilng+1)+0.5) / double (nlng)) - pi;

    double slat = sin(latc), clat = cos(latc);
    double slng = sin(lngc), clng = cos(lngc);

    return vec3d_t(slat*clng, clat, slat*-slng);
}

void SurfaceTile::setSubtextureRange(const tcrd_t &ptcr)
{
    if ((ilng & 1) == 0)
    {   // Right column of tile
        tcRange.tumin = (ptcr.tumin + ptcr.tumax) / 2.0;
        tcRange.tumax = ptcr.tumax;
    }
    else
    {   // Left column of tile
        tcRange.tumin = ptcr.tumin;
        tcRange.tumax = (ptcr.tumin + ptcr.tumax) / 2.0;
    }

    if (ilat & 1)
    {   // Top row of tile
        tcRange.tvmin = (ptcr.tvmin + ptcr.tvmax) / 2.0;
        tcRange.tvmax = ptcr.tvmax;
    }
    else
    {   // Bottom row of tile
        tcRange.tvmin = ptcr.tvmin;
        tcRange.tvmax = (ptcr.tvmin + ptcr.tvmax) / 2.0;
    }
}

void SurfaceTile::load()
{
    uint8_t *ddsImage = nullptr;
    Texture *image = nullptr;
    uint32_t szImage = 0;
    int res;

    state = Loading;

    if (image == nullptr && smgr.zTrees[0] != nullptr)
    {
        res = szImage = smgr.zTrees[0]->read(lod+4, ilat, ilng, &ddsImage);
        if (szImage > 0 && ddsImage != nullptr)
        {
            image = Texture::loadDDSFromMemory(ddsImage, szImage);
            delete [] ddsImage;
        }
    }

    if (image != nullptr)
    {
        txImage = image;
        txOwn = true;
    }
    else
    {
        // Non-existent tile. Have to load lower LOD tile
        // from parent tile and set subtexture range.
        SurfaceTile *pTile = dynamic_cast<SurfaceTile *>(getParent());
        if (pTile != nullptr)
        {
            txImage = pTile->getTexture();
            txOwn = false;
            setSubtextureRange(pTile->tcRange);
        }
    }

    mesh = smgr.createSphere(lod, ilat, ilng, 32, tcRange);
    if (mesh != nullptr)
        mesh->setTexture(txImage);

    state = Inactive;
}

void SurfaceTile::render(renderParam &prm)
{
    if (mesh != nullptr)
        mesh->render();
}

// ******** SurfaceManager ********

SurfaceManager::SurfaceManager(Context &ctx, const Object &object)
: ctx(ctx), object(object)
{
    ShaderManager &smgr = *ctx.getShaderManager();

    pgmPlanet = smgr.createShader("planet");
    
    pgmPlanet->use();
    mvp = mat4Uniform(pgmPlanet->getID(), "mvp");

    pgmPlanet->release();

    for (int idx = 0; idx < 5; idx++)
        zTrees[idx] = nullptr;
    
    switch(object.getType())
    {
    case Object::objCelestialBody:
        const celBody *body = dynamic_cast<const celBody *>(&object);
        const PlanetarySystem *system = body->getInSystem();
        str_t starName = system->getStar()->getsName();
        str_t bodyName = body->getsName();

        surfaceFolder = fmt::sprintf("systems/%s/%s/Orbiter",
            starName, bodyName);
        break;
    }

    zTrees[0] = zTreeManager::create(surfaceFolder, "surf");

    // Initialize root of virtual surface tiles
    for(int idx = 0; idx < 2; idx++)
    {
        tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
        tiles[idx]->load();
    }
}

SurfaceManager::~SurfaceManager()
{
    for (int idx = 0; idx < 2; idx++)
        delete tiles[idx];
}

void SurfaceManager::update(SurfaceTile *tile, renderParam &prm)
{
    tile->state = SurfaceTile::Rendering;
}

void SurfaceManager::render(SurfaceTile *tile, renderParam &prm)
{
    if (tile->state == SurfaceTile::Rendering)
        tile->render(prm);
    else if (tile->state == SurfaceTile::Active)
    {
        for (int idx = 0; idx < 4; idx++)
        {
            SurfaceTile *child = tile->getChild(idx);
            if (child != nullptr && (child->state & TILE_ACTIVE))
                render(child, prm);
        }
    }
}

void SurfaceManager::render(renderParam &prm, ObjectProperties &op)
{
    pgmPlanet->use();

    prm.maxLOD  = 19;
    prm.biasLOD = 0;
    prm.orad    = op.orad;
    prm.oqrot   = op.oqrot;
    prm.orot    = glm::toMat4(prm.oqrot);

    prm.cdir    = prm.orot * vec4d_t(op.opos, 1.0);
    prm.cdist   = glm::length(prm.cdir) / prm.orad;
    prm.viewap  = (prm.cdist >= 1.0) ? acos(1.0 / prm.cdist) : 0.0;
    prm.cdir    = glm::normalize(prm.cdir);
    prm.color   = op.color;

    prm.dmModel = glm::transpose(prm.orot);
    prm.dmWorld = glm::translate(prm.dmView, op.opos);
    prm.mvp     = mat4f_t(prm.dmProj * prm.dmWorld * prm.dmModel);

    mvp = prm.mvp;

    // Updating and rendering virtual tiles
    for (int idx = 0; idx < 2; idx++)
        update(tiles[idx], prm);
    for (int idx = 0; idx < 2; idx++)
        render(tiles[idx], prm);

    pgmPlanet->release();
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
