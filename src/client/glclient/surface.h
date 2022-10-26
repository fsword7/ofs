// surface.h - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#pragma once

#include "utils/tree.h"
#include "shader.h"

#define ELEV_STRIDE 0

class VertexArray;
class VertexBuffer;
class IndexBuffer;

struct Vertex
{
    float vx, vy, vz;   // Vertex position
    float nx, ny, nz;   // Normal position
    float tu, tv;       // Texture coordinates
};

struct Mesh
{
    Mesh(int nvtx, Vertex *vtx, int nidx, uint16_t *idx)
    : nvtx(nvtx), vtx(vtx), nidx(nidx), idx(idx)
    { }

    ~Mesh()
    {
        delete [] vtx;
        delete [] idx;
    }

    void upload();

    VertexArray    *vao = nullptr;;
    VertexBuffer   *vbo = nullptr;
    IndexBuffer    *ibo = nullptr;

    glm::dvec4 *bbvtx;

    int         nvtx;
    Vertex     *vtx;
    int         nidx;
    uint16_t   *idx;
};

struct tcRange
{
    double tumin, tumax;
    double tvmin, tvmax;
};

class Scene;
class SurfaceManager;
class ShaderProgram;

#define TILE_ACTIVE     64
#define TILE_VALID      128
class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
    friend class SurfaceManager;

public:
    enum tileType
    {
        tileInvalid   = 0,
        tileInQueue   = 1,
        tileLoading   = 2,
        tileInactive  = 3|TILE_VALID,
        tileActive    = 4|TILE_VALID|TILE_ACTIVE,
        tileInvisible = 5|TILE_VALID|TILE_ACTIVE,
        tileRendering = 6|TILE_VALID|TILE_ACTIVE
    };

    SurfaceTile(SurfaceManager &mgr, int lod, int ilat, int ilng, SurfaceTile *parent = nullptr);
    ~SurfaceTile();

    SurfaceTile *createChild(int idx);
    glm::dvec3 getCenter();

    void load();
    void render();

    Mesh *createHemisphere(int grid, int16_t *elev, double gelev);
    // Mesh *createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
    //     int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

private:
    SurfaceManager &mgr;

    tileType type = tileInvalid;

    int lod;
    int ilat, ilng;
    glm::dvec3 center;

    Mesh *mesh = nullptr;
};

class SurfaceManager
{
    friend class SurfaceTile;

public:
    SurfaceManager(ObjectHandle object, Scene &scene);
    ~SurfaceManager();

    glm::dmat4 getWorldMatrix(int ilat, int nlat, int ilng, int nlng);

    Mesh *createHemisphere(int grid, int16_t *elev, double gelev);
    Mesh *createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
        int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

    void setRenderParams(const glm::dmat4 &dmWorld);

    void process(SurfaceTile *tile);
    void render(SurfaceTile *tile); 
    void render(const glm::dmat4 &dmWorld);

    struct renderParams
    {
        int maxlod; // Maximum LOD level

        glm::dmat3  urot;           // planet rotation matrix [universe frame]
        glm::dvec3  cpos;           // camera position [planet frame]
        glm::dvec3  cdir;           // camera direction [planet frame]

        double cdist;               // camera distance from center
        double scale;
        double viewap;
    
        glm::dmat4  dmWorld;        // model matrix
        glm::dmat4  dmViewProj;     // view/projection matrix
        
    } prm;

private:
    ObjectHandle object;
    Scene &scene;

    ShaderProgram *pgmBody = nullptr;
    mat4Uniform uViewProj;
    mat4Uniform uView;
    mat4Uniform uModel;

    SurfaceTile *tiles[2];

    double objSize = 0.0;
    double resScale;
    double resBias;
};