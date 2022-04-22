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

    // orbtial positions
    vec3d_t getuPosition(double tjd) override;
    quatd_t getuOrientation(double tjd) override;

    vec3d_t getoPosition(double tjd);
};