// vobject.h - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#pragma once

#include "buffer.h"

class Scene;
class celBody;
class ShaderProgram;
struct ObjectProperties
{
    celBody *body;

    color_t     color;
    uint32_t    maxLOD;
    uint32_t    biasLOD;
    glm::dvec3  opos;
    glm::dmat3  orot;
    double      orad;
    glm::dvec3  wpos;      // world coordinates (longitude/latitude)
    glm::dvec3  lpos;      // local planetocentric coordinates

    glm::dmat4  mvp;
    glm::vec2   clip;

    glm::dvec3  cpos;
    glm::dvec3  cqrot;
    glm::dmat3  crot;
    glm::dvec3  cdir;
    double      cdist;

    double   viewap;
    double   tanap;
    double   mjd;
    double   dTime;
};

struct objVertex
{
    glm::vec3   posObject;
    color_t     color;
    float       size;
};

class vObject
{
public:
    vObject(const Object *obj, Scene &scene);
    virtual ~vObject();

    void init();

    inline const Object *getObject() const   { return object; }

    static vObject *create(const Object *object, Scene &scene);

    virtual void update(int now);
    virtual void render(const ObjectListEntry &ole) {}
    
    void renderObjectAsPoint(const ObjectListEntry &ole);

protected:
    const Object *object;
    Scene &scene;

    glm::dvec3 gpos;
    glm::dmat3 grot;

    // Camera parameters
    glm::dvec3 vpos;
    double     vdist;

    glm::dmat4 dmWorld;

    ShaderProgram *pgmObjectAsPoint = nullptr;

    VertexArray *vao = nullptr;
    VertexBuffer *vbo = nullptr;

    objVertex vtx;

    mat4Uniform mvp;
    vec2Uniform uCamClip;
};