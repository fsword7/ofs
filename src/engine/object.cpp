// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OPSAPI_SERVER_BUILD

#include "main/core.h"
// #include "yaml-cpp/yaml.h"
#include "engine/object.h"
#include "utils/json.h"

Object::Object(json &config, ObjectType type)
: objType(type)
{
    setsName(myjson::getString<str_t>(config, "name", "(unknown)"));
    setMass(myjson::getFloat<double>(config, "mass"));
    setRadius(myjson::getFloat<double>(config, "radius"));
    setAlbedo(myjson::getFloat<double>(config, "geom-albedo"));
}

void Object::update(bool force)
{
}

void Object::beginUpdate()
{
}

void Object::endUpdate()
{
    s0 = s1; // Move all to S0 for on the air
    s1 = {}; // Clear all for next update

    // Set identity for R and Q rotations
    s1.R = glm::dmat3(1.0);
    s1.Q = glm::dquat();
}

glm::dvec3 Object::getuPosition(double tjd) const
{
    return objPosition;
}

glm::dvec3 Object::getuVelocity(double tjd) const
{
    return objPosition;
}

glm::dmat3 Object::getuOrientation(double tjd) const
{
    return objRotation;
}

glm::dquat Object::getqOrientation(double tjd) const
{
    return objqRotation;
}


glm::dvec3 Object::getoPosition(double tjd) const
{
    return objPosition;
}

glm::dvec3 Object::getoVelocity(double tjd) const
{
    return objVelocity;
}

void Object::updateCullingRadius()
{
    cullingRadius = getBoundingRadius();
}

// void Object::getValueReal(json &data, cstr_t &name, double &value)
// {
//     value = data[name].get<double>();
// }

// void Object::getValueString(json &data, cstr_t &name, str_t &value)
// {
//     value = data[name].get<std::string>();
// }