// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

#include "main/core.h"
#include "universe/universe.h"
#include "engine/player.h"
#include "shader.h"
#include "lights.h"

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
struct LightState;

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

// Sun light sources list
struct LightSource
{
    glm::dvec3  spos;           // Sun position;
    double      luminosity;     // Sun Luminosity
    double      radius;         // Sun radius
    color_t     color;          // Color temperature
};

// Reflected object list
struct SecondaryLight
{
    Celestial   *object;        // Reflected object
    glm::dvec3  vpos;           // View position
    double      radius;         // Object radius
    double      reflected;      // Reflected brightness
};

struct ObjectListEntry
{   
    Object  *object = nullptr;     // Object
    vObject *visual = nullptr;

    // Sun and position [km]
    glm::dvec3 vpos;    // Object position (view)
    glm::dmat3 orot;    // Object orientation
    glm::dvec3 spos;    // Sun position

    color_t    color;   // default surface color
    double  objSize;    // Object size

    double  vdist;      // View distance
    double  vSize;      // View object size
    double  pxSize;     // Object size in pixel width
    double  appMag;     // Apparent magnitude

    double  zCenter;    // Center Z depth sorting

    LightState lights;

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

    inline int getWidth() const                     { return width; }
    inline int getHeight() const                    { return height; }

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
    void calculatePointSize(double appMag, double size, double &pointSize, double &alpha);

protected:
    void initStarRenderer();
    void initConstellations();
    void renderStars(double faintest, double mjd);
    void renderConstellations();

    void renderObjectAsPoint(ObjectListEntry &ole);
    void renderCelestialBody(ObjectListEntry &ole);
    void renderOrbitPath(ObjectListEntry &ole);

    void setupPrimaryLightSources(const std::vector<const CelestialStar *> nearStars,
        const glm::dvec3 &obs, std::vector<LightSource> &ls);
    void setupSecondaryLightSources();
    void setObjectLighting(std::vector<LightSource> &suns, const glm::dvec3 &opos,
        const glm::dquat &orot, LightState &ls);

    void updateAutoMag();

    // void buildSystems(FrameTree *tree, const glm::dvec3 &obs,
    //     const glm::dvec3 &vpnorm, const glm::dvec3 &origin);
    void buildSystems(secondaries_t &bodies, const glm::dvec3 &obs,
        const glm::dvec3 &vpnorm);

    void renderSystemObjects();

private:
    int width, height;

    double pixelSize = 0.0;

    double faintestAutoMag45Deg = 8.0;

    double faintestMagNight = 6.0;
    double saturationMagNight = 1.0;

    double faintestMag = 6.0;
    double faintestPlanetMag = 6.0;
    double saturationMag = 1.0;
    double brightnessScale = 1.0;
    double brightnessBias = 0.0;
    double satPoint;

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
    ShaderProgram *pgmObjectAsPoint = nullptr;

    std::vector<vObject *> vobjList;
    std::vector<const CelestialStar *> nearStars;
    std::vector<const CelestialStar *> visibleStars;
    std::vector<LightSource> lightSources;
    std::vector<SecondaryLight> secondaryLights;

    std::vector<ObjectListEntry> renderList;
};