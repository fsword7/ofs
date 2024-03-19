// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OPSAPI_SERVER_BUILD

#include "main/core.h"
// #include "yaml-cpp/yaml.h"
#include "engine/object.h"


Object::Object(YAML::Node &config, ObjectType type)
: objType(type)
{
    std::string name = "(unknown)";

    if (config["Name"].IsScalar())
    {
        setsName(config["Name"].as<std::string>());
        ofsLogger->info("Planet: {}\n", getsName());
    }

    if (config["Mass"].IsScalar())
    {
        setMass(config["Mass"].as<double>());
        ofsLogger->info("Mass: {:f}\n", getMass());
    }

    if (config["Radius"].IsScalar())
    {
        setRadius(config["Radius"].as<double>());
        ofsLogger->info("Radius: {:f}\n", getRadius());
    }

    if (config["GeomAlbedo"].IsScalar())
    {
        setAlbedo(config["GeomAlbedo"].as<double>());
        ofsLogger->info("Albedo: {:f}\n", getAlbedo());
    }

    // if (config["Color"].IsSequence())
    //     geomColor = config["Color"].as<color_t>();
}

void Object::update(bool force)
{
}

void Object::beginUpdate()
{
}

void Object::endUpdate()
{
    s0 = s1; // Move all to S0
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