// vobject.h - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#pragma once

class Scene;
class celBody;

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

    glm::dvec3  cpos;
    glm::dvec3  cqrot;
    glm::dmat3  crot;
    glm::dvec3  cdir;
    double      cdist;

    double   viewap;
    double   tanap;
};

class vObject
{
public:
    vObject(const Object *obj, Scene &scene)
    : object(obj), scene(scene)
    { }
    virtual ~vObject() = default;

    inline const Object *getObject() const   { return object; }

    static vObject *create(const Object *object, Scene &scene);

    virtual void update(int now);
    virtual void render(const ObjectProperties &op) {}

protected:
    const Object *object;
    Scene &scene;

    glm::dvec3 gpos;
    glm::dmat3 grot;

    // Camera parameters
    glm::dvec3 vpos;
    double     vdist;

    glm::dmat4 dmWorld;
};