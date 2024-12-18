// surface.h - Surface tile manager package
//
// Author:  Tim Stark
// Date:    Sep 12, 2022

#pragma once

#include "utils/tree.h"
#include "universe/elevmgr.h"
#include "texmgr.h"
#include "shader.h"

// #define ELEV_STRIDE 0

class VertexArray;
class VertexBuffer;
class IndexBuffer;
class SurfaceTile;
struct ObjectProperties;

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

    VertexArray    *vao = nullptr;
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

#define TILE_ACTIVE     64
#define TILE_VALID      128

class Scene;
class SurfaceHandler;
class SurfaceManager;
class ShaderProgram;

class SurfaceTile : public Tree<SurfaceTile, QTREE_NODES>
{
    friend class SurfaceManager;
    friend class SurfaceHandler;

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

    inline glm::dvec3 getCenter() const     { return center; }
    inline glm::dvec3 getWorld() const      { return wpos; }
    inline glTexture *getTexture() const    { return txImage; }

    bool isInView(const glm::dmat4 &transform);

    SurfaceTile *createChild(int idx);

    void setCenter(glm::dvec3 &center, glm::dvec3 &wpos);
    void setSubregionRange(const tcRange &range);

    void load();
    void render();

    Mesh *createHemisphere(int grid, int16_t *elev, double gelev);
    // Mesh *createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
    //     int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

    void fixLongtitudeBoundary(SurfaceTile *nbr, bool keep = false);
    void fixLatitudeBoundary(SurfaceTile *nbr, bool keep = false);
    void fixCorner(SurfaceTile *nbr);
    void matchEdges();

    double getMeanElevation(const int16_t *elev) const;
    void interpolateElevationGrid(int ilat, int ilng, int lod,
        int pilat, int pilng, int plod, int16_t *pelev, int16_t *elev);
    // int16_t *readElevationFile(int lod, int ilat, int ilng, double eres);
    bool loadElevationData();
    int16_t *getElevationData();

private:
    SurfaceManager &mgr;

    tileType type = tileInvalid;

    int lod;
    int ilat, ilng;
    int nlat, nlng;
    glm::dvec3 center;
    glm::dvec3 wpos;

    SurfaceTile *parentTile = nullptr;
    Mesh *mesh = nullptr;

    // Surface data parameters
    bool txOwn = false;
    glTexture *txImage = nullptr;
    glTexture *spImage = nullptr;
    tcRange txRange;

    // Edge Matching paramaters
    bool     okEdge;
    int      latlod, lnglod, dialod;

    // Elevation data parameters
    bool     elevEnable = true; // for debugging purposes
    bool     elevOwn = false;
    int16_t *elev = nullptr;
    int16_t *ggelev = nullptr;  // Great-grandfather elevation data
    double   elevMean = 0.0;
};

class SurfaceHandler
{
public:
    SurfaceHandler();
    ~SurfaceHandler();

    void start();
    void shutdown();

    void queue(SurfaceTile *tile);
    bool unqueue(SurfaceTile *tile);
    void unqueue(SurfaceManager *mgr);

protected:
    void handle();

private:
    std::queue<SurfaceTile *> tiles;

    volatile bool runHandler;
    int           msFreq;
    std::thread   loader;
    std::mutex    muQueue;
    std::mutex    muLoading;
};

class SurfaceManager
{
    friend class SurfaceTile;

public:
    SurfaceManager(const Object *object, Scene &scene);
    ~SurfaceManager();
    
    static void ginit();
    static void gexit();

    inline int getElevGrid() const { return elevGrids; }
    inline int getElevScale() const { return elevScale; }
    inline int getElevMode() const { return elevMode; }

    SurfaceTile *findTile(int lod, int ilat, int ilng);

    glm::dmat4 getWorldMatrix(int ilat, int nlat, int ilng, int nlng);

    // bool getTileIndex(double lat, double lng, int lod, int &ilat, int &ilng) const;
    // int16_t *readElevationFile(int lod, int ilat, int ilng, double eres) const;
    // double getElevationData(glm::dvec3 loc, int reqlod,
    //     elevTileList_t *elevTiles, glm::dvec3 *nml, int *lod) const;

    // Creating planet surface - quadsphere
    Mesh *createHemisphere(int grid, int16_t *elev, double gelev);
    Mesh *createSpherePatch(int grid, int lod, int ilat, int ilng, const tcRange &range,
        int16_t *elev = nullptr, double selev = 1.0, double gelev = 0.0);

    // Creating star surface - icosphere
    Mesh *createIcosphere(int maxlod);

    void setRenderParams(const ObjectListEntry &ole);

    void process(SurfaceTile *tile);
    void render(SurfaceTile *tile); 

    void renderBody(const ObjectListEntry &ole);
    void renderStar(const ObjectListEntry &ole);

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
        glm::dvec2  clip;           // camera clip for logarthmic depth
   
    } prm;

private:
    const Object *object;
    Scene &scene;

    bool bPolygonLines = false;

    ShaderProgram *pgm = nullptr;
    ShaderProgram *pgmStar = nullptr;
    ShaderProgram *pgmCorona = nullptr;
    ShaderProgram *pgmGlow = nullptr;

    Mesh *meshStar = nullptr;
    Mesh *meshCorona = nullptr;
    Mesh *meshGlow = nullptr;

    mat4Uniform uViewProj;
    mat4Uniform uView;
    mat4Uniform uModel;

    floatUniform uTime;
    floatUniform uRadius;
    vec4Uniform uColor;
    vec3Uniform uCentralDir;
    vec2Uniform uCamClip;

    TextureManager tmgr;
    ElevationManager *emgr = nullptr;

    zTreeManager *zTrees[5] = {};
    SurfaceTile *tiles[2];

    int elevGrids = 32; // 1 << 5
    double elevScale = 1.0;
    int  elevMode = 1;

    float dTime = 0.0;
    
    ObjectType objType = objUnknown;
    double     objSize = 0.0;
    double     resScale;
    double     resBias;

    static SurfaceHandler *loader;
};