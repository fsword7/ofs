// surface.cpp - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#include "main/core.h"
#include "client.h"
#include "camera.h"
#include "scene.h"
#include "buffer.h"
#include "surface.h"

// ******** Surface Tile ********

static tcRange range = { 0, 1, 0, 1 };

SurfaceTile::SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent)
: Tree(parent), mgr(mgr), lod(lod), ilat(ilat), ilng(ilng)
{
    center = getCenter();
}

SurfaceTile::~SurfaceTile()
{
    if (mesh != nullptr)
        delete mesh;
}

glm::dvec3 SurfaceTile::getCenter()
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double cntlat = (pi/2.0) - pi * (double(ilat+0.5)/double(nlat));
    double cntlng = (pi*2.0) * (double(ilng)+0.5 / double(nlng) + pi);
    double slat = sin(cntlat), clat = cos(cntlat);
    double slng = sin(cntlng), clng = cos(cntlng);

    return glm::dvec3(clat*clng, slat, clat*slng);
}

void SurfaceTile::load()
{
    if (lod == 0)
        mesh = createHemisphere(32, nullptr, 0);
    else
        mesh = mgr.createSpherePatch(32, lod, ilat, ilng, range);
    type = tileActive;
}

void SurfaceTile::render()
{
    if (mesh == nullptr)
        return;
    if (mesh->vao == nullptr)
        mesh->upload();

    mgr.pgmBody->use();

    mesh->vao->bind();

    mgr.uViewProj = glm::mat4(mgr.prm.dmViewProj);

    // Load per-tile model matrix into GLSL space
    mgr.uModel = glm::mat4(mgr.prm.dmWorld);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, mesh->ibo->getCount(), GL_UNSIGNED_SHORT, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    mesh->vao->unbind();
    mgr.pgmBody->release();
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
            nml = { slat*clng, clat, slat*slng };
            pos = nml * erad;
            tu = a * x + du;

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
        
        vtx[cvtx].vx = pos.x;
        vtx[cvtx].vy = pos.y;
        vtx[cvtx].vz = pos.z;

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
        
        vtx[cvtx].vx = pos.x;
        vtx[cvtx].vy = pos.y;
        vtx[cvtx].vz = pos.z;

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

    // Regenerate normals for terrain
    if (elev != nullptr)
    {

    }

    return new Mesh(cvtx, vtx, cidx, idx);
}

// ******** Surface Manager ********

SurfaceManager::SurfaceManager(ObjectHandle object, Scene &scene)
: object(object), scene(scene)
{
    ShaderManager &shmgr = scene.getShaderManager();
    pgmBody = shmgr.createShader("body");
    if (pgmBody == nullptr)
        return;

    pgmBody->use();

    uViewProj = mat4Uniform(pgmBody->getID(), "uViewProj");
    uView = mat4Uniform(pgmBody->getID(), "uView");
    uModel = mat4Uniform(pgmBody->getID(), "uModel");

    pgmBody->release();

    objSize = ofsGetObjectRadius(object);

    for (int idx = 0; idx < 2; idx++)
    {
        tiles[idx] = new SurfaceTile(*this, 0, 0, idx);
        tiles[idx]->load();
    }
}

SurfaceManager::~SurfaceManager()
{
    delete tiles[0],tiles[1];
}

glm::dmat4 SurfaceManager::getWorldMatrix(int ilat, int nlat, int ilng, int nlng)
{
    // glm::dmat4 lrot(1.0);

    // lrot = { { clng,  0,   slng, 0   },
    //          { 0,     1.0, 0,    0   },
    //          { -slng, 0,   clng, 0   },  
    //          { 0,     0,   0,    1.0 }};

    if (nlng <= 8)
        return prm.dmWorld;

    double lat = pi * double(nlat/2 - ilat-1) / double(nlat);
    // double lat = pi * double(nlat/2 - ilat) / double(nlat);
    double lng = (pi*2.0) * (double(ilng) / double(nlng)) + pi;
    double slng = sin(lng), clng = cos(lng);

    double dx = objSize * cos(lng) * cos(lat);
    double dy = objSize * sin(lat);
    double dz = objSize * sin(lng) * cos(lat);

    // Calculate translation with per-tile model matrix
    // Move a center from body center to tile by eliminating
    // jiffery due to round-off errors.
    glm::dmat4 wrot = prm.dmWorld;
    wrot[3][0] = (dx*prm.urot[0][0] + dy*prm.urot[0][1] + dz*prm.urot[0][2] + prm.cpos.x) * prm.scale;
    wrot[3][1] = (dx*prm.urot[1][0] + dy*prm.urot[1][1] + dz*prm.urot[1][2] + prm.cpos.y) * prm.scale;
    wrot[3][2] = (dx*prm.urot[2][0] + dy*prm.urot[2][1] + dz*prm.urot[2][2] + prm.cpos.z) * prm.scale;

    return wrot;
}

void SurfaceManager::setRenderParams(const glm::dmat4 &dmWorld)
{
    glm::dvec3 opos, cpos;
    double cdist;

    Camera *cam = scene.getCamera();
    prm.dmViewProj = cam->getViewProjMatrix();
    prm.dmWorld = dmWorld;

    prm.urot = glm::dmat3(1); // ofsGetObjectRotation(object);
    opos = ofsGetObjectGlobalPosition(object);
    cpos = ofsGetCameraGlobalPosition();

    prm.cpos = opos - cpos;
    prm.cdir = tmul(prm.urot, -prm.cpos);
    cdist = glm::length(prm.cdir);
    prm.cdist = cdist / objSize;
    prm.cdir = glm::normalize(prm.cdir);
    prm.viewap = acos(1.0 / (std::max(prm.cdist, 1.0)));
    prm.scale = 1.0;
}

void SurfaceManager::process(SurfaceTile *tile)
{
    int nlat = 1 << tile->lod;
    int nlng = 2 << tile->lod;

    bool bStepdown = true;

    tile->type = SurfaceTile::tileRendering;
    
    static const double rad0 = sqrt(2.0)*(pi/2.0);
    double rad   = rad0 / double(nlat);
    double alpha = acos(glm::dot(prm.cdir, tile->center));
    double adist = alpha - rad;

    if (adist >= prm.viewap)
    {
    }

    // {
    //     double tdist;
    //     double erad = 1.0; // + tile->meanElev/objSize;
    //     if (adist < 0.0)
    //         tdist = prm.cdist - erad;
    //     else
    //     {
    //         double h = erad * sin(adist);
    //         double a = prm.cdist - erad * cos(adist);
    //         tdist = sqrt(a*a + h*h);
    //     }
    //     double apr = tdist * camera->getTanAp();

    //     int tres = (apr < 1e-6 ? prm.maxlod : std::max(0, std::min(prm.maxlod, bias - log(apr)*resScale));
    //     bStepdown = (tile->lod < tres);
    // }

    // if (bStepdown)
    // {
    //     for (int idx = 0; idx < QTREE_NODES; idx++)
    //     {
    //         SurfaceTile *child = tile->getChild(idx);
    //         if (child == nullptr)
    //             tile->loadChild(idx);
    //         else if (child->type == SurfaceTile::Invalid)
    //         {

    //         }
    //     }
    // }

    prm.dmWorld = tile->mgr.getWorldMatrix(tile->ilat, nlat, tile->ilng, nlng);
}

void SurfaceManager::render(SurfaceTile *tile)
{
    if (tile->type == SurfaceTile::tileRendering)
    {
        tile->render();
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

void SurfaceManager::render(const glm::dmat4 &dmWorld)
{
    setRenderParams(dmWorld);

    for (int idx = 0; idx < 2; idx++)
        process(tiles[idx]);

    for (int idx = 0; idx < 2; idx++)
        render(tiles[idx]);
}

Mesh *SurfaceManager::createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
    int16_t *elev, double selev, double gelev)
{
    int nlat = 1 << lod;
    int nlng = 2 << lod;

    double mlat0  = pi * double(nlat/2 - ilat-1) / double(nlat);
    double mlat1  = pi * double(nlat/2 - ilat) / double(nlat);
    double mlng0 = (pi*2.0) * double(ilng) / double(nlng) + pi;
    double mlng1 = (pi*2.0) * double(ilng+1) / double(nlng) + pi;
    // double mlng0  = 0.0;
    // double mlng1  = (pi*2.0) / double(nlng);

    glm::dvec3 pos, nml;
    double  radius = objSize;
    
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

    for (int y = 0; y < grid; y++)
    {
        lat = mlat0 + (mlat1-mlat0) * double(y)/double(grid);       
        slat = sin(lat), clat = cos(lat);
        tu = range.tumin + tur * float(y)/float(grid);

        // logger->debug("Y {}: {}\n", y, lat);

        for (int x = 0; x < grid; x++)
        {
            lng = mlng0 + (mlng1-mlng0) * double(x)/double(grid);
            slng = sin(lng), clng = cos(lng);
            tv = range.tvmin + tvr + float(x)/float(grid);
            erad = radius + gelev;

            if (elev != nullptr)
                erad += double(elev[(y+1)*ELEV_STRIDE + (x+1)]) * selev;
            nml = glm::dvec3(clat*clng, slat, clat*slng);
            pos = nml * erad;

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

    }

    return new Mesh(nvtx, vtx, nidx, idx);
}

// ******** Mesh ********

void Mesh::upload()
{
    vao = new VertexArray();
    vao->bind();

    vbo = new VertexBuffer(vtx, nvtx * sizeof(Vertex));
    vbo->bind();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)12);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)24);
    glEnableVertexAttribArray(2);

    vbo->unbind();

    ibo = new IndexBuffer(idx, nidx);

    vao->unbind();
}