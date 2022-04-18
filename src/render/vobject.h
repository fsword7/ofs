// vobject.h - Visual Object Package
//
// Author:  Tim Stark
// Date:    Apr 17, 2022

#pragma once

#include "engine/object.h"

class Scene;

class vObject
{
public:
    vObject(const Object &obj, Scene &scene)
    : object(obj), scene(scene)
    { }
    virtual ~vObject() = default;

    static vObject *create(const Object &obj, Scene &scene);

    virtual void render() = 0;

protected:
    const Object &object;
    Scene &scene;
};