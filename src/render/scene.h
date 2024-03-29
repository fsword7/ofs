// scene.h - Scene main rendering routines
//
// Author:  Tim Stark
// Date:    Apr 16, 2022

#pragma once

class Context;
class VertexBuffer;
class ShaderManager;
class ShaderProgram;
class Universe;
class FrameTree;
class Object;
class Player;
class celStar;
class celBody;
class StarRenderer;
class StarColors;
class StarDatabase;
class vObject;

#include "osd/gl/lights.h"

template <typename T>
struct Vertex32
{
    T vx, vy, vz;   // vertex
    T nx, ny, nz;   // normal for lightings
    T tu, tv;       // texture coordinates
};

typedef Vertex32<float> vtxf_t;

struct LineVertex
{
    LineVertex(vec3f_t p1, vec3f_t p2, float s)
    : point1(p1), point2(p2), scale(s)
    { }

    vec3f_t point1, point2;
    float scale;
};

struct LineStripVertrex
{
    LineStripVertrex(vec3f_t p, float s)
    : point(p), scale(s)
    { }

    vec3f_t point;
    float scale;
};

template <typename T>
struct TextureCoordRange
{
    T tumin, tumax;
    T tvmin, tvmax;
};

typedef TextureCoordRange<double> tcrd_t;

// Sun light sources list
struct LightSource
{
    vec3d_t spos;       // Sun position;
    double  luminosity; // Sun Luminosity
    double  radius;     // Sun radius
    color_t color;      // Color temperature
};

// Reflected object list
struct SecondaryLight
{
    Object *object;         // Reflected object
    vec3d_t viewPosition;   // View position
    double  radius;         // Object radius
    double  reflected;      // Reflected brightness
};

// Label parameters
struct Annotation
{
    str_t       labelText;
    color_t     color;
    vec3d_t     tpos;
    float       size;

};

struct ObjectListEntry
{   
    Object *object;     // Object

    // Sun and position [km]
    vec3d_t opos;       // Object position
    quatd_t orot;       // Object orientation
    vec3d_t spos;       // Sun position

    double  vdist;      // View distance
    double  objSize;    // Object size in pixel width
    double  appMag;     // Apparent magnitude

    // Clipping parameters
    double  zNear;      // Near Z clipping
    double  zCenter;    // Center Z clipping
    double  zFat;       // Far Z clipping
};

struct ObjectProperties
{
    celBody *body;

    color_t  color;
    uint32_t maxLOD;
    uint32_t biasLOD;
    vec3d_t  opos;
    quatd_t  oqrot;
    mat4d_t  orot;
    double   orad;
    vec3d_t  wpos;      // world coordinates (longitude/latitude)
    vec3d_t  lpos;      // local planetocentric coordinates

    mat4d_t  mvp;

    vec3d_t  cpos;
    vec3d_t  cqrot;
    mat4d_t  crot;
    vec3d_t  cdir;
    double   cdist;

    double   viewap;
    double   tanap;
};

struct renderParam
{
    double jnow; // current Julian date/time

    // View parameters
    vec3d_t cpos;
    quatd_t crot;
    vec3d_t cdir;
    vec3d_t wpos;       // world coordinates (longtidude/latitude)
    double  cdist;
    double  viewap;
    double  tanap;
    double  aspect;
    double  pxSize;     // pixel width

    // Per-object parameters
    int     maxLOD;
    int     biasLOD;
    color_t color;
    double  orad;
    quatd_t oqrot;
    mat4d_t orot;

    mat4d_t dmProj;  // projection matrix
    mat4d_t dmView;  // view matrix
    mat4d_t dmModel; // model matrix
    mat4d_t dmWorld; // Model/view matrix
    mat4d_t dmOrhto; // orthrogaphic matrix for annonations

    mat4f_t mProj;  // projection matrix
    mat4f_t mView;  // view matrix
    mat4f_t mModel; // model matrix
    mat4f_t mWorld; // model/view matrix
    
    mat4f_t mvp;    // total matrix

};

struct VertexLine
{
    vec3f_t spos;
    color_t color;
};

class Scene
{
public:
    Scene(Context &gl) : ctx(gl)
    { }
    ~Scene() = default;

    inline Context &getContext() { return ctx; }
    inline renderParam &getRenderParameters() { return prm; }

    void init(Universe &universe);
    void cleanup();

    void render(Universe &universe, Player &player);

protected:
    void initStarRenderer();
    void initConstellations(Universe &universe);

    void setupPrimaryLightSources(const std::vector<const celStar *> nearStars,
        const vec3d_t &obs, double now, std::vector<LightSource> &ls);
    void setupSecondaryLightSources();
    void setObjectLighting(std::vector<LightSource> &suns, const vec3d_t opos,
        const quatd_t orot, LightState &ls);
  
    void renderStars(const StarDatabase &starlib, const Player &player, double faintest);
    void renderConstellations(Universe &universe, const Player &player);

    void buildNearSystems(FrameTree *tree, Player &player, vec3d_t apos, vec3d_t vpnorm, vec3d_t origin);
    void renderObjectAsPoint(ObjectListEntry &ole);
    void renderCelestialBody(ObjectListEntry &ole);

    vObject *addVisualObject(const Object &object);
    vObject *getVisualObject(const Object &object, bool createFlag);

    vec3d_t getAstrocentericPosition(const celStar *sun, vec3d_t upos, double now);

    void addAnnotation(std::vector<Annotation> &annotations,
        cstr_t &labelText, color_t color, const vec3d_t &pos, float size);

    void renderAnnotations(const std::vector<Annotation> &annotations);

private:
    Context &ctx;
    ShaderManager *smgr = nullptr;

    std::vector<vObject *> vObjectList;
    std::vector<const celStar *> closeStars;
    std::vector<LightSource> lightSources;
    std::vector<ObjectListEntry> objectList;

    // Annotation lists
    std::vector<Annotation> bgAnnotations;      // Background annotation list
    std::vector<Annotation> fgAnnotations;      // Foreground annotation list
    std::vector<Annotation> dsAnnotations;      // Depth-sorted annotation list
    std::vector<Annotation> objAnnotations;     // Object annotation list

    StarRenderer *starRenderer = nullptr;
    StarColors *starColors = nullptr;

    double pixelSize = 1.0;

    float faintestMag = 6.0f;
    float faintestNightMag = faintestMag;
    float saturationMag = 1.0f;

    renderParam prm;

    ShaderProgram *pgmStar = nullptr;
    VertexBuffer *vbufStar = nullptr;

    ShaderProgram *pgmAsterism = nullptr;
    VertexBuffer  *vbufAsterism = nullptr;
    uint32_t asterismLines = 0;
};