// vobject.h - Visual Object Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

#include "engine/object.h"

class Scene;
class Object;

class vObject
{
public:
    vObject(const Object &obj, Scene &scene)
    : object(obj), scene(scene)
    { }
    virtual ~vObject() = default;

    inline const Object *getObject() const { return &object; }

    static vObject *create(const Object &obj, Scene &scene);

    virtual void render(renderParam &prm, ObjectProperties &op);
    virtual void render(renderParam &prm, ObjectProperties &op, LightState &lights);

protected:
    const Object &object;
    Scene &scene;
};