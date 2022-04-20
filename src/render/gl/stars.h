// stars.h - Procedural star renderer package
//
// Author:  Tim Stark
// Date:    Apr 19, 2022

#pragma once

class Scene;
class StarColors;
class Context;
class celStar;
class renderParam;

class StarVertex
{
    friend class Scene;
    friend class StarRenderer;

public:
    enum PointType
    {
        useNotUsed = 0,
        usePoints,
        useSprites
    };

    struct starVertex
    {
        vec3f_t     posStar;
        color_t     color;
        float       size;
    };

    StarVertex(Scene &scene);
    ~StarVertex() = default;

    // Render routines
    void start();
    void render();
    void finish();

    void addStar(const vec3d_t &pos, const color_t &color, double radius);

protected:
    Scene &scene;
    Context &ctx;
    renderParam &prm;

    PointType type;
    int nStars;
    int flagStarted;

    starVertex *vertices = nullptr;

    ShaderProgram *pgm = nullptr;
    VertexBuffer *vbuf = nullptr;

    mat4Uniform mvp;
};

class StarRenderer : public ofsHandler
{
public:
    StarRenderer() = default;
    ~StarRenderer() = default;

    void process(const celStar &star, double dist, double appMag) const;

public:
    vec3d_t obsPos = { 0, 0, 0 }; // Observer's camera position

    ShaderProgram *pgm = nullptr;
    VertexBuffer *vbuf = nullptr;

    // Star buffer for rendering
    StarVertex *starBuffer = nullptr;
    StarColors *starColors = nullptr;

    Scene *scene = nullptr;
    Context *ctx = nullptr;
    vec3d_t cpos; // current camera/player position
    double  pxSize; // pixel width
    double  baseSize;

    float faintestMag = 0.0f;
    float faintestNightMag = 0.0f;
    float saturationMag = 0.0f;
};