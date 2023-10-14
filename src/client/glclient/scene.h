// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

#include "main/core.h"
#include "universe/universe.h"
#include "engine/player.h"
#include "shader.h"

class Camera;
class StarRenderer;
class StarColors;
class ShaderProgram;
class VertexArray;
class VertexBuffer;
class vObject;

class FrameTree;
class Object;
class CelestialStar;


struct LineVertex
{
    LineVertex(glm::vec3 p1, glm::vec3 p2, float s)
    : point1(p1), point2(p2), scale(s)
    { }

    glm::vec3 point1, point2;
    float scale;
};

struct LineStripVertrex
{
    LineStripVertrex(glm::vec3 p, float s)
    : point(p), scale(s)
    { }

    glm::vec3 point;
    float scale;
};

struct AsterismVertex
{
    glm::vec3 spos;
    color_t   color;
};

struct ObjectListEntry
{   
    Object  *object = nullptr;     // Object
    vObject *visual = nullptr;

    // Sun and position [km]
    glm::dvec3 opos;    // Object position
    glm::dmat3 orot;    // Object orientation
    glm::dvec3 spos;    // Sun position

    color_t    color;   // default surface color
    double  objSize;    // Object size

    double  vdist;      // View distance
    double  vSize;      // View object size
    double  pxSize;     // Object size in pixel width
    double  appMag;     // Apparent magnitude

    double  zCenter;    // Center Z depth sorting

    // Clipping parameters
    glm::vec2 camClip;
    double  zFar;       // Far Z clipping
    double  zNear;      // Near Z clipping

    double mjd;         // MJD time/date
};

class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    inline ShaderManager &getShaderManager()        { return shmgr; }
    inline Camera *getCamera() const                { return camera; }
    inline Player *getObserver() const              { return observer; }

    inline void addRenderList(ObjectListEntry &ole) { renderList.push_back(ole); }

    void init(Universe *universe);
    void start();
    void update(Player *player);
    void render(Player *player);
    
    void resize(int width, int height);

    void checkErrors();

    vObject *addVisualObject(const Object *object);
    vObject *getVisualObject(const Object *object, bool bCreate = false);

    glm::dvec3 getAstrocentricPosition(const Object *object, const glm::dvec3 &vpos, int time);

protected:
    void initStarRenderer();
    void initConstellations();
    void renderStars(double faintest, double mjd);
    void renderConstellations();

    void renderObjectAsPoint(ObjectListEntry &ole);
    void renderCelestialBody(ObjectListEntry &ole);

    // void buildSystems(FrameTree *tree, const glm::dvec3 &obs,
    //     const glm::dvec3 &vpnorm, const glm::dvec3 &origin);
    void buildSystems(secondaries_t &bodies, const glm::dvec3 &obs,
        const glm::dvec3 &vpnorm);

    void renderSystemObjects();

private:
    int width, height;

    double pixelSize = 0.0;

    double faintestMag = 6.0;
    double saturationMag = 0.0;

    int now = 0;
    double mjd = 0;

    ShaderManager shmgr;

    Universe *universe = nullptr;
    Player *observer = nullptr;
    Camera *camera = nullptr;

    StarRenderer *starRenderer = nullptr;
    StarColors *starColors = nullptr;

    ShaderProgram *pgmAsterism = nullptr;
    VertexArray *vaoAsterism = nullptr;
    VertexBuffer *vboAsterism = nullptr;
    uint32_t asterismLines = 0;
    mat4Uniform mvp;
    vec2Uniform uCamClip;

    ShaderProgram *pgmStar = nullptr;

    std::vector<vObject *> vobjList;
    std::vector<const CelestialStar *> nearStars;
    std::vector<const CelestialStar *> visibleStars;

    std::vector<ObjectListEntry> renderList;
};