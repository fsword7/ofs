// scene.h - Scene renderer package
//
// Author:  Tim Stark
// Date:    Sep 3, 2022

#pragma once

#include "shader.h"

class Camera;
class StarRenderer;
class StarColors;
class ShaderProgram;
class VertexBuffer;
class vObject;
class Scene
{
public:
    Scene(int width, int height);
    ~Scene() = default;

    inline ShaderManager &getShaderManager()        { return shmgr; }
    inline Camera *getCamera(int idx = 0) const     { return camera; }

    void init();
    void start();
    void update();
    void render();
    
    void checkErrors();

    vObject *addVisualObject(ObjectHandle object);
    vObject *getVisualObject(ObjectHandle object, bool bCreate = false);

protected:
    void initStarRenderer();
    void renderStars(double faintest);

private:
    int width, height;

    double pixelSize = 0.0;

    double faintestMag = 6.0;
    double saturationMag = 0.0;

    ShaderManager shmgr;

    Camera *camera = nullptr;

    StarRenderer *starRenderer = nullptr;
    StarColors *starColors = nullptr;

    ShaderProgram *pgmStar = nullptr;
    VertexBuffer *vbufStar = nullptr;

    std::vector<vObject *> vobjList;
    std::vector<ObjectHandle> nearStars;
    std::vector<ObjectHandle> visibleStars;

    vObject *vEarth = nullptr;
};