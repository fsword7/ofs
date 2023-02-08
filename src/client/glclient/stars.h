// stars.h - Procedural Star Renderer package
//
// Author:  Tim Stark
// Date:    Oct 10, 2022

#pragma once

#include "universe/handle.h"

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
        glm::vec3   posStar;
        color_t     color;
        float       size;
    };

    StarVertex(Scene &scene);
    ~StarVertex() = default;

    // Render routines
    void start();
    void render();
    void finish();

    void addStar(const glm::dvec3 &pos, const color_t &color, double radius);

protected:
    Scene &scene;
    // renderParam &prm;

    PointType type;
    int nStars;
    int flagStarted;

    starVertex *vertices = nullptr;

    ShaderProgram *pgm = nullptr;
    VertexArray *vao = nullptr;
    VertexBuffer *vbo = nullptr;

    mat4Uniform mvp;
};

class StarRenderer : public ofsHandler
{
public:
    StarRenderer() = default;
    ~StarRenderer() = default;

    void process(const CelestialStar &star, double dist, double appMag) const;

public:
    glm::dvec3 obsPos = { 0, 0, 0 }; // Observer's camera position

    ShaderProgram *pgm = nullptr;
    VertexBuffer *vbuf = nullptr;

    // Star buffer for rendering
    StarVertex *starBuffer = nullptr;
    StarColors *starColors = nullptr;

    Scene *scene = nullptr;
    glm::dvec3 cpos; // current camera/player position
    double  pxSize; // pixel width
    double  baseSize;

    float faintestMag = 0.0f;
    float faintestNightMag = 0.0f;
    float saturationMag = 0.0f;
};