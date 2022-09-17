// vobject.h - Visual object package
//
// Author:  Tim Stark
// Date:    Sep 6, 2022

#pragma once

class Scene;

class vObject
{
public:
    vObject(ObjectHandle obj, Scene &scene)
    : object(obj), scene(scene)
    { }
    virtual ~vObject() = default;

    inline ObjectHandle getObject() const   { return object; }

    static vObject *create(ObjectHandle object, Scene &scene);

    virtual void update();
    virtual void render() {}

protected:
    ObjectHandle object;
    Scene &scene;

    glm::dmat3 grot;

    // Camera parameters
    glm::dvec3 cpos;
    glm::dvec3 cdist;

    glm::dmat4 dmWorld;
};