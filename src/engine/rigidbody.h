// rigidbody.h - rigid body package
//
// Author:  Tim Stark
// Date:    Apr 21, 2022

#pragma once

#include "engine/object.h"

class RigidBody : public Object
{
public:
    RigidBody(cstr_t &name, ObjectType type)
    : Object(name, type)
    { }
    virtual ~RigidBody() = default;

};