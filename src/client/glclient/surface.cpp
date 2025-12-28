// surface.cpp - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/player.h"
#include "universe/star.h"
#include "universe/body.h"
#include "universe/elevmgr.h"
#include "client.h"
// #include "camera.h"
#include "scene.h"
#include "buffer.h"
#include "vobject.h"
#include "surface.h"

#include <unordered_map>

// ******** Surface Tile ********

static tcRange range = { 0, 1, 0, 1 };

SurfaceTile::SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent)
: Tree(parent), mgr(mgr), lod(lod), ilat(ilat), nlat(1 << lod), ilng(ilng), nlng(2 << lod),
  txRange(range)
{
    setCenter(cnml, wpos);
    center = cnml * mgr.objSize;
}

SurfaceTile::~SurfaceTile()
{
    if (mesh != nullptr)
        delete mesh;
    if (txOwn == true && txImage != nullptr)
        delete txImage;
    if (txOwn == true && spImage != nullptr)
        delete spImage;
    if (elevOwn == true && elev != nullptr)
        delete elev;
}

SurfaceTile *SurfaceTile::createChild(int idx)
{
    SurfaceTile *child = nullptr;

    int nlod = lod + 1;
    int nlat = ilat*2 + (idx / 2);
    int nlng = ilng*2 + (idx % 2);

    child = new SurfaceTile(mgr, nlod, nlat, nlng, this);
    // if (child != nullptr)
    //     mgr.loader->queue(child);
    child->load();
    addChild(idx, child);

    return child;
}

void SurfaceTile::setCenter(glm::dvec3 &cnml, glm::dvec3 &wpos)
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double latc = (pi/2.0) - pi * ((double(ilat)+0.5) / double(nlat));
    double lngc = (pi*2.0) * ((double(ilng)+0.5) / double(nlng)) + pi;
    double slat = sin(latc), clat = cos(latc);
    double slng = sin(lngc), clng = cos(lngc);

    cnml = glm::dvec3(clat*clng, slat, clat*-slng);
    wpos = glm::dvec3(acos(cnml.y) - (pi / 2.0), atan2(cnml.z, -cnml.x), 0);
}

bool SurfaceTile::isInView(const glm::dmat4 &transform)
{
    return false;
}

void SurfaceTile::setSubregionRange(const tcRange &range)
{
    if (ilng & 1)
    {   // Right column of tile
        txRange.tumin = (range.tumin + range.tumax) / 2.0;
        txRange.tumax = range.tumax;
    }
    else
    {   // Left column of tile
        txRange.tumin = range.tumin;
        txRange.tumax = (range.tumin + range.tumax) / 2.0;
    }

    if (ilat & 1)
    {   // Top row of tile
        txRange.tvmin = (range.tvmin + range.tvmax) / 2.0;
        txRange.tvmax = range.tvmax;
    }
    else
    {   // Bottom row of tile
        txRange.tvmin = range.tvmin;
        txRange.tvmax = (range.tvmin + range.tvmax) / 2.0;
    }
}

void SurfaceTile::load()
{
    type = tileLoading;
    uint32_t szImage;

    uint8_t *ddsImage = nullptr;

    if (mgr.zTrees[0] != nullptr)
    {
        // Loading terrain texture from database
        szImage = mgr.zTrees[0]->read(lod+4, ilat, ilng, &ddsImage);
        if (szImage > 0 && ddsImage != nullptr)
            txImage = mgr.tmgr.loadDDSTextureFromMemory(ddsImage, szImage, 0);
        // if (txImage != nullptr)
        //     logger->info("Loaded texture (ID {}: ({}, {}))\n",
        //         txImage->id, txImage->txWidth, txImage->txHeight);
        delete ddsImage;

        if (txImage != nullptr)
            txOwn = true;
        else {
            // Non-existent tile. Get lower LOD tile from
            // ancestor and set subregion range of that.
            SurfaceTile *pTile = dynamic_cast<SurfaceTile *>(getParent());
            if (pTile != nullptr)
            {
                txImage = pTile->getTexture();
                txOwn = false;
                setSubregionRange(pTile->txRange);

                // Get parent tile with last own texture image.
                parentTile = pTile->txOwn ? pTile : pTile->parentTile;
            }
        }
    }

    // // Load (nightlight/water) mask texture if applicance
    // if (mgr.zTrees[1] != nullptr)
    // {
    //     if (txOwn == true) {
    //         // Loading specular texture from database
    //         szImage = mgr.zTrees[1]->read(lod+4, ilat, ilng, &ddsImage);
    //         if (szImage > 0 && ddsImage != nullptr)
    //             mgr.tmgr.loadDDSTextureFromMemory(&spImage, ddsImage, szImage, 0);
    //         // if (txImage != nullptr)
    //         //     logger->info("Loaded texture (ID {}: ({}, {}))\n",
    //         //         txImage->id, txImage->txWidth, txImage->txHeight);
    //         delete ddsImage;
    //     } else {
    //         SurfaceTile *pTile = dynamic_cast<SurfaceTile *>(getParent());
    //         if (pTile != nullptr)
    //             spImage = pTile->spImage;
    //     }
    // }

    // Load elevation data
    int16_t *elev = elevEnable ? getElevationData() : nullptr;

    if (lod == 0)
        mesh = createHemisphere(mgr.elevGrids, elev, mgr.elevScale);
    else
        mesh = mgr.createSpherePatch(mgr.elevGrids, lod, ilat, ilng,
            (lod >= 4), center, txRange, elev, mgr.elevScale, 0.0);
    type = tileInactive;
}

// void SurfaceTile::getTwoFloats(const glm::dvec3 &val, glm::fvec3 &high, glm::fvec3 &low)
// {
//     // high = glm::fvec3(val);
//     // low = glm::fvec3(val - glm::dvec3(high));
//     high.x = float(val.x);
//     high.y = float(val.y);
//     high.z = float(val.z);
//     low.x = float(val.x - high.x);
//     low.y = float(val.y - high.y);
//     low.z = float(val.z - high.z);
// }

void SurfaceTile::render()
{
    if (mesh == nullptr)
        return;
    if (mesh->vao == nullptr)
    {
        if (type & TILE_VALID)
            mesh->upload();
        else
            return;
    }

    mgr.pgm->use();
    mesh->vao->bind();

    if (txImage != nullptr)
    {
        // logger->info("Yes, here 3\n");
        glActiveTexture(GL_TEXTURE0);
        txImage->bind();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    // mgr.uViewProj = glm::mat4(mgr.prm.dmViewProj);

    // Load per-tile model matrix into GLSL space
    // mgr.uModel = glm::mat4(mgr.prm.dmWorld);
    mgr.uModel = glm::mat4(mgr.prm.dmWorld);
    // mgr.uView = glm::mat4(mgr.prm.dmView);
    // mgr.uWorld = glm::mat4(mgr.prm.dmProj * mgr.prm.dmRTE);
    mgr.uWorld = glm::mat4(mgr.prm.dmProj * mgr.prm.dmWorldt);
    // mgr.uRTE = mgr.prm.dmViewProj * mgr.prm.dmRTE;
    // mgr.uRTE = mgr.prm.dmRTE * mgr.prm.dmView * mgr.prm.dmProj;
    // mgr.uRTE = mgr.prm.dmProj * mgr.prm.dmView * mgr.prm.dmRTE;

    // glm::vec3 high, low;
    // getTwoFloats(mgr.prm.cpos, high, low);
    // mgr.uCamEyeHigh = high;
    // mgr.uCamEyeLow = low;

    mgr.uCamClip = mgr.prm.clip;

    // if (mgr.bPolygonLines)
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, mesh->ibo->getCount(), GL_UNSIGNED_SHORT, 0);
    // if (mgr.bPolygonLines)
    //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (txImage != nullptr)
    {
        glDisable(GL_CULL_FACE);
        txImage->unbind();
    }
    
    mesh->vao->unbind();
    mgr.pgm->release();
}

void SurfaceTile::renderNormals()
{
    if (mesh == nullptr)
        return;
    mgr.pgmNormals->use();
    mesh->vao->bind();

    mgr.unWorld = glm::mat4(mgr.prm.dmWorldt);
    mgr.unProj = glm::mat4(mgr.prm.dmProj);
    mgr.unColor = { 1.0, 1.0, 0.0, 1.0 };
    mgr.unCamClip = mgr.prm.clip;

    glDrawElements(GL_TRIANGLES, mesh->ibo->getCount(), GL_UNSIGNED_SHORT, 0);

    mesh->vao->unbind();
    mgr.pgmNormals->release();
}

Mesh *SurfaceTile::createHemisphere(int grid, int16_t *elev, double gelev)
{
    double erad, rad = mgr.objSize + gelev;
    double slat, clat, slng, clng;
    double tu, tv;
    glm::dvec3 pos, nml;

    int nvtx = (grid - 1) * (grid + 1) + 2;
    Vertex *vtx = new Vertex[nvtx];
    int cvtx = 0;

    double dang = pi / grid;
    double lng, lat = 0;
    double du = 0.5 / 512;
    double a = (1.0 - 2.0 * du) / double(grid);
    int x1 = grid;
    int x2 = x1+1;

    for (int y = 1; y < grid; y++)
    {
        lat += dang;
        slat = sin(lat), clat = cos(lat);
        tv = lat / pi;

        for (int x = 0; x < x2; x++)
        {
            lng = x*dang - pi;
            if (ilng != 0)
                lng += pi;
            slng = sin(lng), clng = cos(lng);
            erad = rad + gelev;
            if (elev != nullptr)
                erad += double(elev[(grid+1 - y) * ELEV_STRIDE + x+1]);
            nml = { slat*clng, clat, slat*-slng };
            pos = nml * erad;
            tu = a * x + du;

            // // vertex (high)
            // vtx[cvtx].vxh = float(pos.x);
            // vtx[cvtx].vyh = float(pos.y);
            // vtx[cvtx].vzh = float(pos.z);
            // // vertex (low)
            // vtx[cvtx].vxl = float(pos.x - vtx[cvtx].vxh);
            // vtx[cvtx].vyl = float(pos.y - vtx[cvtx].vyh);
            // vtx[cvtx].vzl = float(pos.z - vtx[cvtx].vzh);

            // vertex
            vtx[cvtx].vx = float(pos.x);
            vtx[cvtx].vy = float(pos.y);
            vtx[cvtx].vz = float(pos.z);

            vtx[cvtx].nx = float(nml.x);
            vtx[cvtx].ny = float(nml.y);
            vtx[cvtx].nz = float(nml.z);

            vtx[cvtx].tu = tu;
            vtx[cvtx].tv = tv;
   
            cvtx++;
        }
    }

    // Regenerate normals for terrain
    // if (elev != nullptr)
    // {
    //     // double dy, dz, dydz, nx1, ny1, nz1;
    //     int en;
    //     float escale = 1.0;

    //     // dy = rad * pi/grid;
    //     for (int y = 1; y < grid; y++)
    //     {
    //         lat = (pi/2.0) - y * f0ang;
    //         // slat = sin(lat), clat = cos(lat);
    //         // dz = rad * pi * cos(lat) / grid;
    //         // dydz = dy*dz;
    //         for (int x = 0; x < x2; x++)
    //         {
    //             lng = x * f0ang;
    //             if (!lng)
    //                 lng -= pi;
    //             // slng = sin(lng), clng = cos(lng);
    //             en = (grid+1-y)*ELEV_STRIDE + x+1;
    //             nml = { escale*(elev[en-1]-elev[en+1]),
    //                     escale*(elev[en+ELEV_STRIDE]-elev[en-ELEV_STRIDE]),
    //                     2.0};
    //             nml = glm::normalize(nml);
    //             nml = ofs::xRotate(lat) * ofs::hRotate(lng) * nml;

    //             vtx->nx = nml.x;
    //             vtx->ny = nml.y;
    //             vtx->nz = nml.z;

    //             vtx++;
    //         }
    //     }
    // }

    // Initialize indices
    int nidx = 6 * (grid * (grid - 2) + grid);
    uint16_t *idx = new uint16_t[nidx];
    int cidx = 0;

    for (int y = 0; y < grid-2; y++)
    {
        for (int x = 0; x < x1; x++)
        {
            idx[cidx++] = (uint16_t)((y+0)*x2 + (x+0));
            idx[cidx++] = (uint16_t)((y+0)*x2 + (x+1));
            idx[cidx++] = (uint16_t)((y+1)*x2 + (x+0));
            idx[cidx++] = (uint16_t)((y+0)*x2 + (x+1));
            idx[cidx++] = (uint16_t)((y+1)*x2 + (x+1));
            idx[cidx++] = (uint16_t)((y+1)*x2 + (x+0));
        }
    }
    
    // Make top and bottom
    int wNorth = cvtx;
    {
        erad = rad + gelev;
        if (elev != nullptr)
        {
            double mn = 0.0;
            for (int x = 0; x < x2; x++)
                mn += elev[(grid+1)*ELEV_STRIDE + x+1];
            erad += mn / x2;
        }
        nml = { 0, 1, 0 };
        pos = nml * erad;
        
        // // vertex (high)
        // vtx[cvtx].vxh = pos.x;
        // vtx[cvtx].vyh = pos.y;
        // vtx[cvtx].vzh = pos.z;
        // // vertex (low)
        // vtx[cvtx].vxl = float(pos.x - vtx[cvtx].vxh);
        // vtx[cvtx].vyl = float(pos.y - vtx[cvtx].vyh);
        // vtx[cvtx].vzl = float(pos.z - vtx[cvtx].vzh);

        vtx[cvtx].vx = float(pos.x);
        vtx[cvtx].vy = float(pos.y);
        vtx[cvtx].vz = float(pos.z);

        vtx[cvtx].nx = nml.x;
        vtx[cvtx].ny = nml.y;
        vtx[cvtx].nz = nml.z;

        vtx[cvtx].tu = 0.5;
        vtx[cvtx].tv = 0.0;

        cvtx++;
    }

    int wSouth = cvtx;
    {
        erad = rad + gelev;
        if (elev != nullptr)
        {
            double mn = 0.0;
            for (int x = 0; x < x2; x++)
                mn += elev[ELEV_STRIDE + x+1];
            erad += mn / x2;
        }
        nml = { 0, -1, 0 };
        pos = nml * erad;
        
        // // vertex (high)
        // vtx[cvtx].vxh = pos.x;
        // vtx[cvtx].vyh = pos.y;
        // vtx[cvtx].vzh = pos.z;
        // // vertex (low)
        // vtx[cvtx].vxl = float(pos.x - vtx[cvtx].vxh);
        // vtx[cvtx].vyl = float(pos.y - vtx[cvtx].vyh);
        // vtx[cvtx].vzl = float(pos.z - vtx[cvtx].vzh);

        vtx[cvtx].vx = float(pos.x);
        vtx[cvtx].vy = float(pos.y);
        vtx[cvtx].vz = float(pos.z);

        vtx[cvtx].nx = nml.x;
        vtx[cvtx].ny = nml.y;
        vtx[cvtx].nz = nml.z;

        vtx[cvtx].tu = 0.5;
        vtx[cvtx].tv = 1.0;

        cvtx++;
    }

    for (int x = 0; x < x1; x++)
    {
        idx[cidx++] = wNorth;
        idx[cidx++] = (0)*x2 + (x+1);
        idx[cidx++] = (0)*x2 + (x+0);
    }

    for (int x = 0; x < x1; x++)
    {
        idx[cidx++] = wSouth;
        idx[cidx++] = (grid-2)*x2 + (x+0);
        idx[cidx++] = (grid-2)*x2 + (x+1);
    }

 
    return new Mesh(cvtx, vtx, cidx, idx);
}

// ******** Surface Manager ********

// Global parameters
SurfaceHandler *SurfaceManager::loader = nullptr;

static ShaderPackage glslStar[] = 
{
    { "star.vs.glsl", true, shrVertexProcessor },
    { "star.fs.glsl", true, shrFragmentProcessor }
};

static ShaderPackage glslCorona[] = 
{
    { "corona.vs.glsl", true, shrVertexProcessor },
    { "corona.fs.glsl", true, shrFragmentProcessor }
};

static ShaderPackage glslBody[] = 
{
    { "body.vs.glsl", true, shrVertexProcessor },
    { "body.fs.glsl", true, shrFragmentProcessor }
};

static ShaderPackage glslNormals[] =
{
    { "normals.vs.glsl", true, shrVertexProcessor },
    { "normals.gs.glsl", true, shrGeometryProcessor },
    { "normals.fs.glsl", true, shrFragmentProcessor }
};

SurfaceManager::SurfaceManager(const Object *object, Scene &scene)
: object(object), scene(scene)
{
    ShaderManager &shmgr = scene.getShaderManager();

    objType = object->getType();
    objSize = object->getRadius();

    switch (objType)
    {
    case objCelestialStar:
        pgm = shmgr.createShader("star", glslStar, ARRAY_SIZE(glslStar));
        if (pgm == nullptr)
            return;

        pgmCorona = shmgr.createShader("corona", glslCorona, ARRAY_SIZE(glslCorona));
        if (pgmCorona == nullptr)
            return;

        pgm->use();

        uViewProj = mat4Uniform(pgm->getID(), "uViewProj");
        // uView = mat4Uniform(pgm->getID(), "uView");
        uModel = mat4Uniform(pgm->getID(), "uModel");
        uCamClip = vec2Uniform(pgm->getID(), "uCamClip");

        uRadius = floatUniform(pgm->getID(), "uRadius");
        uColor = vec4Uniform(pgm->getID(), "uColor");
        uCentralDir = vec3Uniform(pgm->getID(), "uCentralDir");
        uTime = floatUniform(pgm->getID(), "uTime");
    
        pgm->release();

        pgmCorona->use();

        // glm::vec2 cvtx[4] = {
        //     { -objSize,  objSize },
        //     { -objSize, -objSize },
        //     {  objSize, -objSize },
        //     {  objSize,  objSize }
        // };

        // uint16_t cidx[6] = { 0, 1, 2, 2, 3, 0 };

        // meshCorona = new Mesh(4, cvtx, 6, cidx);
    
        pgmCorona->release();

        for (int idx = 0; idx < 2; idx++)
        {
            tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
            tiles[idx]->load();
        }
        // meshStar = createIcosphere(4);
        break;
  
    case objCelestialBody:
        pgm = shmgr.createShader("body", glslBody, ARRAY_SIZE(glslBody));
        if (pgm == nullptr)
            return;

        pgmNormals = shmgr.createShader("normals", glslNormals, ARRAY_SIZE(glslNormals));

        pgm->use();

        pgm->initLightParameters();
        // uViewProj = mat4Uniform(pgm->getID(), "uViewProj");
        // uView = mat4Uniform(pgm->getID(), "uView");
        uModel = mat4Uniform(pgm->getID(), "uModel");
        uWorld = mat4Uniform(pgm->getID(), "uWorld");

        // uCamEyeHigh = vec3Uniform(pgm->getID(), "uCamEyeHigh");
        // uCamEyeLow = vec3Uniform(pgm->getID(), "uCamEyeLow");
        uCamClip = vec2Uniform(pgm->getID(), "uCamClip");

        pgm->release();

        pgmNormals->use();

        unColor = vec4Uniform(pgmNormals->getID(), "uColor");
        unCamClip = vec2Uniform(pgmNormals->getID(), "uCamClip");
        unWorld = mat4Uniform(pgmNormals->getID(), "uWorld");
        unProj = mat4Uniform(pgmNormals->getID(), "uProj");

        pgmNormals->release();
    
        bPolygonLines = true;

        const CelestialPlanet *body = dynamic_cast<const CelestialPlanet *>(object);

        str_t starName = body->getStar()->getsName();
        str_t bodyName = body->getsName();

        // fs::path folder = fmt::format("{}orbiter", body->getPath());
        fs::path folder = body->getPath() + "/orbiter";
    
        emgr = body->getElevationManager();
        // emgr->setup(folder);

        zTrees[0] = zTreeManager::create(folder, "surf");
        zTrees[1] = zTreeManager::create(folder, "mask");
        // zTrees[0] = emgr->getZTreeDatabase(0); // surface
        // zTrees[1] = emgr->getZTreeDatabase(1); // mask

        for (int idx = 0; idx < 2; idx++)
        {
            tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
            tiles[idx]->load();
        }
        break;
    }
}

SurfaceManager::~SurfaceManager()
{
    delete tiles[0],tiles[1];
}

void SurfaceManager::ginit()
{
    loader = new SurfaceHandler();
}

void SurfaceManager::gexit()
{
    if (loader != nullptr)
    {
        delete loader;
        loader = nullptr;
    }
}

SurfaceTile *SurfaceManager::findTile(int lod, int ilat, int ilng)
{
    int nlng = 2 << lod;
    if (ilng < 0)
        ilng += nlng;
    else if (ilng >= nlng)
        ilng -= nlng;
    
    SurfaceTile *tile = tiles[(ilng >> lod) & 1];
    for (int ilod = lod-1; ilod >= 0; ilod--)
    {
        int slat, slng, cidx;
    
        if (tile->type == SurfaceTile::tileInvisible)
            return nullptr;
        slat = (ilat >> ilod) & 1;
        slng = (ilng >> ilod) & 1;
        cidx = slat*2+slng;
        SurfaceTile *child = tile->getChild(cidx);
        if (child == nullptr || (child->type & TILE_ACTIVE) == 0)
            break;
        tile = child;
    }

    return tile;
}

// Set world matrix with using RTC (relative to center) and
// apply it to each tile during rendering. Geometry objects
// must be within a boundary radius of 131,071 meters to avoid
// jittering due to roundoff errors. Also subtract tile vertices
// with tile center.
//
// For more information, read page 164-169 on "3D Engine Design for
// Virtual Globes" book. 

glm::dmat4 SurfaceManager::getWorldMatrix(const SurfaceTile *tile)
{
    glm::dmat4 dmWorld = prm.dmView * prm.dmWorld;

    // Return with RTW method as default.
    if (tile->lod < 4)
        return dmWorld;

    // Set RTC world matrix with tile center.
    dmWorld[3] = dmWorld * glm::dvec4(tile->center, 1.0);

    return dmWorld;
}

void SurfaceManager::setRenderParams(const ObjectListEntry &ole)
{
    glm::dvec3 opos, cpos;
    double cdist;
    Camera *camera = scene.getCamera();
    Player *player = scene.getObserver();

    prm.maxlod = 16;
    resScale = 1400.0 / double(camera->getHeight());

    prm.urot = ole.orot;

    // prm.dmViewProj = camera->getProjMatrix() * camera->getViewMatrix();
    // prm.dmWorld = dmWorld;
    prm.dmViewProj = camera->getProjViewMatrix();
    prm.dmView = camera->getViewMatrix();
    prm.dmProj = camera->getProjMatrix();

    // logMatrix(prm.dmViewProj, "View/Projection");
    // logMatrix(prm.dmWorld, "Model");

    // prm.urot = glm::dmat3(1); // ofsGetObjectRotation(object);
    // opos = object->getuPosition(0);
    // opos = object->getoPosition();
    // cpos = player->getPosition(); //  camera->getGlobalPosition();

    prm.cpos = ole.vpos; // opos - cpos;
    prm.cdir = tmul(prm.urot, -prm.cpos);
    cdist = glm::length(prm.cdir);
    prm.cdist = cdist / objSize;
    prm.cdir = glm::normalize(prm.cdir);
    prm.viewap = acos(1.0 / (std::max(prm.cdist, 1.0)));
    prm.scale = 1.0;
    prm.clip = ole.camClip; //camera->getClip();

    // prm.dmWorld = glm::dmat4(prm.urot);
    // prm.dmWorld = glm::translate(prm.dmWorld, prm.cpos);
    prm.dmWorld = { prm.urot[0][0], prm.urot[1][0], prm.urot[2][0], 0,
                    prm.urot[0][1], prm.urot[1][1], prm.urot[2][1], 0,
                    prm.urot[0][2], prm.urot[1][2], prm.urot[2][2], 0,
                    prm.cpos.x,     prm.cpos.y,     prm.cpos.z,     1 };

    // prm.dmRTE = prm.dmView * prm.dmWorld;
    // prm.dmRTE[3] = { 0, 0, 0, 1 };

    // prm.dmRTE   = { prm.urot[0][0], prm.urot[1][0], prm.urot[2][0], 0,
    //                 prm.urot[0][1], prm.urot[1][1], prm.urot[2][1], 0,
    //                 prm.urot[0][2], prm.urot[1][2], prm.urot[2][2], 0,
    //                 0.0,            0.0,            0.0,            1 };

    // prm.dmRTE = glm::mat4(prm.urot);

    // logger->debug("Object name:     {}\n", object->getName());
    // logger->debug("Object position: {},{},{}\n", opos.x, opos.y, opos.z);
    // logger->debug("Camera position: {},{},{}\n", prm.cpos.x, prm.cpos.y, prm.cpos.z);
    // logger->debug("Camera distance: {}\n", prm.cdist);
}

void SurfaceManager::process(SurfaceTile *tile)
{
    static const double scale = 1.1;

    int nlat = 1 << tile->lod;
    int nlng = 2 << tile->lod;

    bool bStepdown = true;

    tile->type = SurfaceTile::tileRendering;
    
    static const double trad0 = sqrt(3.0)*(pi/2.0);
    double trad  = trad0 / double(nlat);
    double alpha = acos(glm::dot(prm.cdir, tile->cnml));
    double adist = alpha - trad;
    double bias = 4;

    // logger->debug("View {:.6f} >= {:.6f}\n", adist, prm.viewap);

    if (adist >= prm.viewap)
    {
        {
            // logger->debug("Tile: LOD {} ({},{}) - processing\n",
            //     tile->lod+4, tile->ilat, tile->ilng);

            // logger->debug(" -- Center: {:.6f} {:.6f} {:.6f} - {:.6f}{} {:.6f}{}\n",
            //     tile->center.x, tile->center.y, tile->center.z,
            //     abs(ofs::degrees(tile->wpos.x)), (tile->wpos.x < 0) ? 'S' : 'N',
            //     abs(ofs::degrees(tile->wpos.y)), (tile->wpos.y < 0) ? 'W' : 'E');
            // logger->debug(" -- Direction: {:.6f} {:.6f} {:.6f}\n",
            //     prm.cdir.x, prm.cdir.y, prm.cdir.z);        
            // logger->debug(" -- Out of view {:.6f} >= {:.6f} - rendering at LOD 1 level\n",
            //     ofs::degrees(adist), ofs::degrees(prm.viewap));
        }

        if (tile->lod > 0)
        {
            // if (tileDebug)
            // {
            //     logger->debug("Tile: LOD {} ({},{}) - processing\n",
            //         tile->lod+4, tile->ilat, tile->ilng);

            //     Logger::getLogger()->debug(" -- Center: {:.6f} {:.6f} {:.6f} - {:.6f}{} {:.6f}{}\n",
            //         tile->center.x, tile->center.y, tile->center.z,
            //         abs(ofs::degrees(tile->wpos.x())), (tile->wpos.x() < 0) ? 'S' : 'N',
            //         abs(ofs::degrees(tile->wpos.y())), (tile->wpos.y() < 0) ? 'W' : 'E');
        
            //     logger->debug(" -- Out of view {:.6f} >= {:.6f} - rendering at LOD 1 level\n",
            //         ofs::degrees(adist), ofs::degrees(prm.viewap));
            // }

            tile->type = SurfaceTile::tileInvisible;
            return;
        }
        else
            bStepdown = false;
    }

    if (bStepdown == true)
    {
        double tdist;
        double erad = 1.0; // + tile->meanElev/objSize;
        if (adist < 0.0)
            tdist = prm.cdist - erad;
        else
        {
            double h = erad * sin(adist);
            double a = prm.cdist - erad * cos(adist);
            tdist = sqrt(a*a + h*h);
        }
        double apr = tdist * scene.getCamera()->getAperature(); // * resScale;

        int tres = apr < 1e-6 ? prm.maxlod : std::max(0, std::min(prm.maxlod, int(bias - log(apr) * scale)));
        // logger->debug("lod = {}, tres = {}\n", tile->lod, tres);
        bStepdown = (tile->lod < tres);
    }

    if (bStepdown == true)
    {
        bool valid = true;

        for (int idx = 0; idx < QTREE_NODES; idx++)
        {
            SurfaceTile *child = tile->getChild(idx);
            if (child == nullptr)
                child = tile->createChild(idx);
            else if (child->type == SurfaceTile::tileInvalid)
                child->load();
            if ((child->type & TILE_VALID) == 0)
                valid = false;
        }

        if (valid == true)
        {
            tile->type = SurfaceTile::tileActive;
            for (int idx = 0; idx < QTREE_NODES; idx++)
                process(tile->getChild(idx));
            return;
        }

        {
            // logger->debug("Tile: LOD {} ({},{}) - processing\n",
            //     tile->lod+4, tile->ilat, tile->ilng);

            // logger->debug(" -- Center: {:.6f} {:.6f} {:.6f} - {:.6f}{} {:.6f}{}\n",
            //     tile->center.x, tile->center.y, tile->center.z,
            //     abs(ofs::degrees(tile->wpos.x)), (tile->wpos.x < 0) ? 'S' : 'N',
            //     abs(ofs::degrees(tile->wpos.y)), (tile->wpos.y < 0) ? 'W' : 'E');
            // logger->debug(" -- Direction: {:.6f} {:.6f} {:.6f}\n",
            //     prm.cdir.x, prm.cdir.y, prm.cdir.z);
            // logger->debug(" -- Alpha: {:.6f} => Radius {:.6f}, Distance {:.6f}\n",
            //     ofs::degrees(alpha), ofs::degrees(trad), ofs::degrees(adist));
            // if (tile->parentTile != nullptr)
            //     logger->debug(" -- Using tile LOD {} ({},{}) with last available image\n",
            //         tile->parentTile->lod+4, tile->parentTile->ilat, tile->parentTile->ilng);
            // logger->debug(" -- In view {:.6f} < {:.6f} - rendering\n",
            //     ofs::degrees(adist), ofs::degrees(prm.viewap));
        }
    }

    // prm.dmWorldt = tile->mgr.getWorldMatrix(tile->ilat, nlat, tile->ilng, nlng);
    // prm.dmWorld = tile->mgr.getWorldMatrix2(tile);

    // Release all old tiles.
    if (bStepdown == false)
        tile->deleteChildren();
}

void SurfaceManager::render(SurfaceTile *tile)
{
    if (tile->type == SurfaceTile::tileRendering)
    {
        // tile->matchEdges();
        prm.dmWorldt = getWorldMatrix(tile);
        tile->render();
        if (showNormals)
            tile->renderNormals();
    }
    else if (tile->type == SurfaceTile::tileActive)
    {
        for (int idx = 0; idx < QTREE_NODES; idx++)
        {
            SurfaceTile *child = tile->getChild(idx);
            if (child != nullptr && child->type & TILE_ACTIVE)
                render(child);
        }
    }
}

void SurfaceManager::renderBody(const ObjectListEntry &ole)
{
    setRenderParams(ole);

    for (int idx = 0; idx < 2; idx++)
        process(tiles[idx]);

    pgm->use();
    // Set light source parameters
    pgm->setLightParameters(ole.lights);
    pgm->release();

    for (int idx = 0; idx < 2; idx++)
        render(tiles[idx]);
    // pgm->release();
}

void SurfaceManager::renderStar(const ObjectListEntry &ole)
{
    setRenderParams(ole);

    pgm->use();
    
    // Updating time for solar granules aninmation
    dTime += 0.0002;

    uViewProj = glm::mat4(prm.dmViewProj);
    uModel = glm::mat4(prm.dmWorld);
    uRadius = objSize;
    uColor = ole.color.vec4();
    uCentralDir = prm.cdir;
    uCamClip = ole.camClip;
    uTime = dTime;

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // glDrawElements(GL_TRIANGLES, meshStar->ibo->getCount(), GL_UNSIGNED_SHORT, 0);
    // checkErrors();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (int idx = 0; idx < 2; idx++)
        tiles[idx]->render();

    // meshStar->vao->unbind();
    pgm->release();
}

Mesh *SurfaceManager::createSpherePatch(int grid, int lod, int ilat, int ilng, 
    bool rtcEnable, const glm::dvec3 &center, const tcRange &range,
    int16_t *elev, double escale, double gelev)
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double mlat0  = pi * double(nlat/2 - ilat-1) / double(nlat);
    double mlat1  = pi * double(nlat/2 - ilat) / double(nlat);
    double mlng0 = (pi*2.0) * double(ilng) / double(nlng) + pi;
    double mlng1 = (pi*2.0) * double(ilng+1) / double(nlng) + pi;

    glm::dvec3 pos, nml;
    double radius = objSize;

    double slat, clat, slng, clng;
    double lat, lng;
    double erad;

    // Initialize vertices
    int nvtx  = (grid+1)*(grid+1);
    int nvtxe = nvtx + grid+1 + grid+1;
    Vertex *vtx = new Vertex[nvtxe];
    int cvtx = 0;

    float tur = range.tumax - range.tumin;
    float tvr = range.tvmax - range.tvmin;
    float tu, tv;

    // double clat0 = cos(mlat0), slat0 = sin(mlat0);
    // double clat1 = cos(mlat1), slat1 = sin(mlat1);
    // double clng0 = cos(mlng0), slng0 = sin(mlng0);
    // double clng1 = cos(mlng1), slng1 = sin(mlng1);
    // glm::dvec3 ex = glm::normalize(glm::dvec3( clat0*clng1 - clat0*clng0, 0, clat0*slng1 - clat0*slng0 ));
    // glm::dvec3 ey = glm::normalize(glm::dvec3( (clng0+clng1)*(clat1-clat0)*0.5, slat1-slat0, (slng0+slng1)*(clat1-clat0)*0.5 ));
    // glm::dvec3 ez = glm::cross(ey, ex);

    for (int y = 0; y <= grid; y++)
    {
        lat = mlat0 + (mlat1-mlat0) * double(y)/double(grid);       
        slat = sin(lat), clat = cos(lat);
        tv = range.tvmax - tvr * float(y)/float(grid);

        for (int x = 0; x <= grid; x++)
        {
            lng = mlng0 + (mlng1-mlng0) * double(x)/double(grid);
            slng = sin(lng), clng = cos(lng);
            tu = range.tumin + tur * float(x)/float(grid);
            erad = radius + gelev;

            if (elev != nullptr)
                erad += (double(elev[(y+1)*ELEV_STRIDE + (x+1)]) * escale) / 1000.0;
            nml = glm::dvec3(clat*clng, slat, clat*-slng);
            pos = nml * erad;

            // Subtract vertices with tile center
            // for RTC world matrix to eliminate
            // jittering. See getWorldMatrix()
            // above.

            if (rtcEnable)
                pos -= center;

            // // vertex (high)
            // vtx[cvtx].vxh = float(pos.x);
            // vtx[cvtx].vyh = float(pos.y);
            // vtx[cvtx].vzh = float(pos.z);
            // // vertex (low)
            // vtx[cvtx].vxl = float(pos.x - vtx[cvtx].vxh);
            // vtx[cvtx].vyl = float(pos.y - vtx[cvtx].vyh);
            // vtx[cvtx].vzl = float(pos.z - vtx[cvtx].vzh);
         
            // vertex (high)
            vtx[cvtx].vx = float(pos.x);
            vtx[cvtx].vy = float(pos.y);
            vtx[cvtx].vz = float(pos.z);

            vtx[cvtx].nx = float(nml.x);
            vtx[cvtx].ny = float(nml.y);
            vtx[cvtx].nz = float(nml.z);

            vtx[cvtx].tu = tu;
            vtx[cvtx].tv = tv;
   
            cvtx++;
        }
    }

    // Initialize indices
    int nidx = 2 * grid*grid * 3;
    uint16_t *idx = new uint16_t[nidx];
    int cidx = 0;

    if (elev != nullptr)
    {
        int16_t *elev1, *elev1n; 
        int16_t *elev2, *elev2n;
        int16_t err1, err2;

        for (int y = 0, nofs0 = 0; y < grid; y++)
        {
            int nofs1 = nofs0+grid+1;
            for (int x = 0; x < grid; x++)
            {
                elev1  = elev + ELEV_STRIDE+2 + x+ELEV_STRIDE*y;
                elev2  = elev1 + ELEV_STRIDE-1;
                elev1n = elev1 - ELEV_STRIDE+1;
                elev2n = elev2 + ELEV_STRIDE-1;
                err1 = abs(*elev1*2 - *elev2 - *elev1n) + abs(*elev2*2 - *elev1 - *elev2n);
                
                elev1  = elev + ELEV_STRIDE+1 + x+ELEV_STRIDE*y;
                elev2  = elev1 + ELEV_STRIDE+1;
                elev1n = elev1 - ELEV_STRIDE-1;
                elev2n = elev2 + ELEV_STRIDE+1;
                err2 = abs(*elev1*2 - *elev2 - *elev1n) + abs(*elev2*2 - *elev1 - *elev2n);
              
                if (err1 < err2)
                {
                    idx[cidx++] = nofs0+x;
                    idx[cidx++] = nofs1+x;
                    idx[cidx++] = nofs0+x+1;
                    idx[cidx++] = nofs0+x+1;
                    idx[cidx++] = nofs1+x;
                    idx[cidx++] = nofs1+x+1;
                }
                else
                {
                    idx[cidx++] = nofs0+x;
                    idx[cidx++] = nofs1+x+1;
                    idx[cidx++] = nofs0+x+1;
                    idx[cidx++] = nofs1+x+1;
                    idx[cidx++] = nofs0+x;
                    idx[cidx++] = nofs1+x;
                }
            }
            nofs0 = nofs1;
        }
    }
    else
    {
        for (int y = 0, nofs0 = 0; y < grid; y++)
        {
            int nofs1 = nofs0+grid+1;
            for (int x = 0; x < grid; x++)
            {
                idx[cidx++] = nofs0+x;
                idx[cidx++] = nofs1+x;
                idx[cidx++] = nofs0+x+1;
                idx[cidx++] = nofs1+x+1;
                idx[cidx++] = nofs0+x+1;
                idx[cidx++] = nofs1+x;
            }
            nofs0 = nofs1;
        }
    }

    if (elev != nullptr)
    {
        double dy, dz, dydz;
        glm::dmat3 lhrot;
        int en;

        dy = radius * pi/(nlat*grid);
        for (int y = 0, n = 0; y <= grid; y++)
        {
            lat = mlat0 + (mlat1-mlat0) * double(y)/double(grid);       
            slat = sin(lat), clat = cos(lat);
            dz = radius * pi2*clat / (nlng*grid);
            dydz = dy*dz;
            for (int x = 0; x <= grid; x++)
            {
                lng = mlng0 + (mlng1-mlng0) * double(x)/double(grid);
                slng = sin(lng+pi/2), clng = cos(lng+pi/2);
                en = (y+1)*ELEV_STRIDE + (x+1);

                // nml = { escale*(elev[en+1]-elev[en-1]),
                //         escale*(elev[en+ELEV_STRIDE]-elev[en-ELEV_STRIDE]),
                //         2.0};
                // nml = { 1, 0, 0 };
                nml = { 2.0,
                        escale*(elev[en+ELEV_STRIDE]-elev[en-ELEV_STRIDE]),
                        escale*(elev[en+1]-elev[en-1])};
                nml = glm::normalize(-nml);
                nml = ofs::yRotate(-lng-pi) * ofs::zRotate(lat) * nml;

                vtx[n].nx = nml.x;
                vtx[n].ny = nml.y;
                vtx[n].nz = nml.z;
                n++;
            }
        }
    }

    for (int idx = 0, cvtx = nvtx; idx <= grid; idx++)
        vtx[cvtx++] = vtx[idx*(grid+1) + ((ilng & 1) ? grid : 0)];
    for (int idx = 0; idx <= grid; idx++)
        vtx[cvtx++] = vtx[idx + ((ilat & 1) ? 0 : (grid+1)*grid)];

    return new Mesh(nvtx, vtx, nidx, idx);
}

// Icosphere mesh creation

// Seed of Andromeda Icosphere Generator
// Written by Frank McCoy
// Use it for whatever, but remember where you got it from.

// http://www.songho.ca/opengl/gl_sphere.html
// https://en.wikipedia.org/wiki/Regular_icosahedron
// https://en.wikipedia.org/wiki/Geodesic_polyhedron

const static double GR = (1 + sqrt(5)) / 2; // Golden Ratio
const static int nIcosahedronVertices = 12; // 10T + 2
const static glm::dvec3 IcosahedronVertices[12] =
{
    glm::dvec3(-1.0, GR, 0.0),
    glm::dvec3(1.0, GR, 0.0),
    glm::dvec3(-1.0, -GR, 0.0),
    glm::dvec3(1.0, -GR, 0.0),

    glm::dvec3(0.0, -1.0, GR),
    glm::dvec3(0.0, 1.0, GR),
    glm::dvec3(0.0, -1.0, -GR),
    glm::dvec3(0.0, 1.0, -GR),

    glm::dvec3(GR, 0.0, -1.0),
    glm::dvec3(GR, 0.0, 1.0),
    glm::dvec3(-GR, 0.0, -1.0),
    glm::dvec3(-GR, 0.0, 1.0)
};

const static int nIcosahedronIndices = 60;
const static uint32_t IcosahedronIndices[60] =
{
    0, 11, 5,
    0, 5, 1,
    0, 1, 7,
    0, 7, 10,
    0, 10, 11,

    1, 5, 9,
    5, 11, 4,
    11, 10, 2,
    10, 7, 6,
    7, 1, 8,

    3, 9, 4,
    3, 4, 2,
    3, 2, 6,
    3, 6, 8,
    3, 8, 9,

    4, 9, 5,
    2, 4, 11,
    6, 2, 10,
    8, 6, 7,
    9, 8, 1
};

class dvec3Keys
{
public:
    size_t operator () (const glm::dvec3 &k) const
    {
        return std::hash<double>()(k.x) ^ std::hash<double>()(k.y) ^ std::hash<double>()(k.z);
    }

    bool operator () (const glm::dvec3 &a, const glm::dvec3 &b) const
    {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

Mesh *SurfaceManager::createIcosphere(int maxlod)
{
    std::vector<uint32_t>   indices;
    std::vector<glm::dvec3> vertices;

    std::unordered_map<glm::dvec3, uint32_t, dvec3Keys, dvec3Keys> vertexLookup;

    indices.resize(nIcosahedronIndices);
    for (auto index : IcosahedronIndices)
        indices.push_back(index);
    
    vertices.resize(nIcosahedronVertices);
    for (auto pos : IcosahedronVertices)
        vertices.push_back(pos);
    
    glLogger->debug("Creating icosphere...\n");

    for (size_t lod = 0; lod < maxlod; lod++)
    {
        glLogger->debug("Creating LOD {} level...\n", lod);

        std::vector<uint32_t> newIndices;

        glLogger->debug("Indices = {}\n", indices.size());

        for (int idx = 0; idx < indices.size(); idx += 3)
        {
            glm::dvec3 v1 = vertices[indices[idx + 0]];
            glm::dvec3 v2 = vertices[indices[idx + 1]];
            glm::dvec3 v3 = vertices[indices[idx + 2]];

            glm::dvec3 mp12 = getMidpoints(v1, v2);
            glm::dvec3 mp23 = getMidpoints(v2, v3);
            glm::dvec3 mp13 = getMidpoints(v1, v3);

            uint32_t mp12idx, mp23idx, mp13idx;

            auto iter = vertexLookup.find(mp12);
            if (iter != vertexLookup.end())
                mp12idx = iter->second;
            else 
            {
                mp12idx = vertices.size();
                vertices.push_back(mp12);
                vertexLookup[mp12] = mp12idx;
            }

            iter = vertexLookup.find(mp23);
            if (iter != vertexLookup.end())
                mp23idx = iter->second;
            else 
            {
                mp23idx = vertices.size();
                vertices.push_back(mp23);
                vertexLookup[mp23] = mp23idx;
            }

            iter = vertexLookup.find(mp13);
            if (iter != vertexLookup.end())
                mp13idx = iter->second;
            else 
            {
                mp13idx = vertices.size();
                vertices.push_back(mp13);
                vertexLookup[mp13] = mp13idx;
            }

            newIndices.push_back(indices[idx]);
            newIndices.push_back(mp12idx);
            newIndices.push_back(mp13idx);

            newIndices.push_back(mp12idx);
            newIndices.push_back(indices[idx+1]);
            newIndices.push_back(mp23idx);

            newIndices.push_back(mp13idx);
            newIndices.push_back(mp23idx);
            newIndices.push_back(indices[idx+2]);

            newIndices.push_back(mp12idx);
            newIndices.push_back(mp23idx);
            newIndices.push_back(mp13idx);
        }

        glLogger->debug("Swapping indices...\n");

        indices.swap(newIndices);
        newIndices.clear();
    }

    int nvtx = vertices.size();
    int nidx = indices.size();

    glLogger->debug("Icosphere: {} vertices, {} indices\n", nvtx, nidx);

    Vertex *vtx   = new Vertex[nvtx];
    uint16_t *idx = new uint16_t[nidx];

    for (int ivtx = 0; ivtx < nvtx; ivtx++)
    {
        glLogger->debug("Vertex {:04d}: {} {} {}\n" ,ivtx, vertices[ivtx].x, vertices[ivtx].y, vertices[ivtx].z);

        vtx[ivtx].vx = float(vertices[ivtx].x * objSize);
        vtx[ivtx].vy = float(vertices[ivtx].y * objSize);
        vtx[ivtx].vz = float(vertices[ivtx].z * objSize);

        vtx[ivtx].nx = float(vertices[ivtx].x);
        vtx[ivtx].ny = float(vertices[ivtx].y);
        vtx[ivtx].nz = float(vertices[ivtx].z);

        vtx[ivtx].tu = 0.0;
        vtx[ivtx].tv = 0.0;
    }

    for (int iidx = 0; iidx < nidx; iidx++)
    {
        // logger->debug("Index {:04d}: {} {} {}\n" , iidx, 
        idx[iidx] = indices[iidx];
    }

    glLogger->debug("Done - Creating mesh...\n");

    return new Mesh(nvtx, vtx, nidx, idx);
    
    // logger->debug("All done.\n");
}

// /// Seed of Andromeda Icosphere Generator
// /// Written by Frank McCoy
// /// Use it for whatever, but remember where you got it from.

// #include <cstdint>
// #include <unordered_map>
// #include <vector>
// #include <glm/glm.hpp>
// // GLM is awesome ^ ^ ^

// const static float GOLDEN_RATIO = 1.61803398875f;

// const static int NUM_ICOSOHEDRON_VERTICES = 12;
// const static glm::vec3 ICOSOHEDRON_VERTICES[12] = {
//     glm::vec3(-1.0f, GOLDEN_RATIO, 0.0f),
//     glm::vec3(1.0f, GOLDEN_RATIO, 0.0f),
//     glm::vec3(-1.0f, -GOLDEN_RATIO, 0.0f),
//     glm::vec3(1.0f, -GOLDEN_RATIO, 0.0f),

//     glm::vec3(0.0f, -1.0f, GOLDEN_RATIO),
//     glm::vec3(0.0f, 1.0f, GOLDEN_RATIO),
//     glm::vec3(0.0f, -1.0, -GOLDEN_RATIO),
//     glm::vec3(0.0f, 1.0f, -GOLDEN_RATIO),

//     glm::vec3(GOLDEN_RATIO, 0.0f, -1.0f),
//     glm::vec3(GOLDEN_RATIO, 0.0f, 1.0f),
//     glm::vec3(-GOLDEN_RATIO, 0.0f, -1.0f),
//     glm::vec3(-GOLDEN_RATIO, 0.0, 1.0f)
// };

// const static int NUM_ICOSOHEDRON_INDICES = 60;
// const static uint32_t ICOSOHEDRON_INDICES[60] = {
//     0, 11, 5,
//     0, 5, 1,
//     0, 1, 7,
//     0, 7, 10,
//     0, 10, 11,

//     1, 5, 9,
//     5, 11, 4,
//     11, 10, 2,
//     10, 7, 6,
//     7, 1, 8,

//     3, 9, 4,
//     3, 4, 2,
//     3, 2, 6,
//     3, 6, 8,
//     3, 8, 9,

//     4, 9, 5,
//     2, 4, 11,
//     6, 2, 10,
//     8, 6, 7,
//     9, 8, 1
// };

// // Hash functions for the unordered map
// class Vec3KeyFuncs {
// public:
//     size_t operator()(const glm::vec3& k)const {
//         return std::hash<float>()(k.x) ^ std::hash<float>()(k.y) ^ std::hash<float>()(k.z);
//     }

//     bool operator()(const glm::vec3& a, const glm::vec3& b)const {
//         return a.x == b.x && a.y == b.y && a.z == b.z;
//     }
// };

// inline glm::vec3 findMidpoint(glm::vec3 vertex1, glm::vec3 vertex2) {
//     return glm::normalize(glm::vec3((vertex1.x + vertex2.x) / 2.0f, (vertex1.y + vertex2.y) / 2.0f, (vertex1.z + vertex2.z) / 2.0f));
// }

// /// Generates an icosphere with radius 1.0f.
// /// @param lod: Number of subdivisions
// /// @param indices: Resulting indices for use with glDrawElements
// /// @param positions: Resulting vertex positions
// void generateIcosphereMesh(size_t lod, std::vector<uint32_t>& indices, std::vector<glm::vec3>& positions) {
//     std::vector<uint32_t> newIndices;
//     newIndices.reserve(256);

//     std::unordered_map<glm::vec3, uint32_t, Vec3KeyFuncs, Vec3KeyFuncs> vertexLookup;
    
//     indices.resize(NUM_ICOSOHEDRON_INDICES);
//     for (uint32_t i = 0; i < NUM_ICOSOHEDRON_INDICES; i++) {
//         indices[i] = ICOSOHEDRON_INDICES[i];
//     }
//     positions.resize(NUM_ICOSOHEDRON_VERTICES);
//     for (uint32_t i = 0; i < NUM_ICOSOHEDRON_VERTICES; i++) {
//         positions[i] = glm::normalize(ICOSOHEDRON_VERTICES[i]);
//         vertexLookup[glm::normalize(ICOSOHEDRON_VERTICES[i])] = i;
//     }

//     for (size_t i = 0; i < (size_t)lod; i++) {
//         for (size_t j = 0; j < indices.size(); j += 3) {
//             /*
//             j
//             mp12   mp13
//             j+1    mp23   j+2
//             */
//             // Defined in counter clockwise order
//             glm::vec3 vertex1 = positions[indices[j + 0]];
//             glm::vec3 vertex2 = positions[indices[j + 1]];
//             glm::vec3 vertex3 = positions[indices[j + 2]];

//             glm::vec3 midPoint12 = findMidpoint(vertex1, vertex2);
//             glm::vec3 midPoint23 = findMidpoint(vertex2, vertex3);
//             glm::vec3 midPoint13 = findMidpoint(vertex1, vertex3);

//             uint32_t mp12Index;
//             uint32_t mp23Index;
//             uint32_t mp13Index;

//             auto iter = vertexLookup.find(midPoint12);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp12Index = iter->second;
//             } else { // Not in the map
//                 mp12Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint12);
//                 vertexLookup[midPoint12] = mp12Index;
//             }

//             iter = vertexLookup.find(midPoint23);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp23Index = iter->second;
//             } else { // Not in the map
//                 mp23Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint23);
//                 vertexLookup[midPoint23] = mp23Index;
//             }

//             iter = vertexLookup.find(midPoint13);
//             if (iter != vertexLookup.end()) { // It is in the map
//                 mp13Index = iter->second;
//             } else { // Not in the map
//                 mp13Index = (uint32_t)positions.size();
//                 positions.push_back(midPoint13);
//                 vertexLookup[midPoint13] = mp13Index;
//             }

//             newIndices.push_back(indices[j]);
//             newIndices.push_back(mp12Index);
//             newIndices.push_back(mp13Index);

//             newIndices.push_back(mp12Index);
//             newIndices.push_back(indices[j + 1]);
//             newIndices.push_back(mp23Index);

//             newIndices.push_back(mp13Index);
//             newIndices.push_back(mp23Index);
//             newIndices.push_back(indices[j + 2]);

//             newIndices.push_back(mp12Index);
//             newIndices.push_back(mp23Index);
//             newIndices.push_back(mp13Index);
//         }
//         indices.swap(newIndices);
//         newIndices.clear();
//     }
// }

// ******** Mesh ********

void Mesh::upload()
{
    if (vao == nullptr)
        vao = new VertexArray();
    vao->bind();

    if (vbo != nullptr)
        delete vbo;
    vbo = new VertexBuffer(vtx, nvtx * sizeof(Vertex), GL_STATIC_DRAW);
    vao->addVertices(vbo);
    vbo->bind();

    // // Vertex (high)
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // glEnableVertexAttribArray(0);
    // checkErrors();

    // // Vertex (low)
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)12);
    // glEnableVertexAttribArray(1);
    // checkErrors();

    // Vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);
    checkErrors();

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)12);
    glEnableVertexAttribArray(1);
    checkErrors();

    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)24);
    glEnableVertexAttribArray(2);
    checkErrors();

    if (ibo != nullptr)
        delete ibo;
    ibo = new IndexBuffer(idx, nidx);
    vao->addIndices(ibo);
    ibo->bind();

    vao->unbind();
}