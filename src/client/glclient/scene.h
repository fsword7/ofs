// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

#include "main/core.h"
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
    Object *object;     // Object

    // Sun and position [km]
    glm::dvec3 opos;    // Object position
    glm::dmat3 orot;    // Object orientation
    glm::dvec3 spos;    // Sun position

    double  vdist;      // View distance
    double  objSize;    // Object size in pixel width
    double  appMag;     // Apparent magnitude

    // Clipping parameters
    double  zNear;      // Near Z clipping
    double  zCenter;    // Center Z clipping
    double  zFat;       // Far Z clipping
};

class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    inline ShaderManager &getShaderManager()        { return shmgr; }
    inline Camera *getCamera() const                { return camera; }

    void init();
    void start();
    void update(Player *player);
    void render(Player *player);
    
    void resize(int width, int height);

    void checkErrors();

    vObject *addVisualObject(ObjectHandle object);
    vObject *getVisualObject(ObjectHandle object, bool bCreate = false);

    glm::dvec3 getAstrocentricPosition(ObjectHandle object, const glm::dvec3 &vpos, int time);

protected:
    void initStarRenderer();
    void initConstellations();
    void renderStars(double faintest);
    void renderConstellations();

    void renderObjectAsPoint(ObjectListEntry &ole);
    void renderCelestialBody(ObjectListEntry &ole);

    void buildSystems(FrameTree *tree, const glm::dvec3 &obs,
        const glm::dvec3 &vpnorm, const glm::dvec3 &origin);

private:
    int width, height;

    double pixelSize = 0.0;

    double faintestMag = 6.0;
    double saturationMag = 0.0;

    int now = 0;

    ShaderManager shmgr;

    Camera *camera = nullptr;

    StarRenderer *starRenderer = nullptr;
    StarColors *starColors = nullptr;

    ShaderProgram *pgmAsterism = nullptr;
    VertexArray *vaoAsterism = nullptr;
    VertexBuffer *vboAsterism = nullptr;
    uint32_t asterismLines = 0;
    mat4Uniform mvp;

    ShaderProgram *pgmStar = nullptr;

    std::vector<vObject *> vobjList;
    std::vector<ObjectHandle> nearStars;
    std::vector<ObjectHandle> visibleStars;

    // vObject *vEarth = nullptr;
};