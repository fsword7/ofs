// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#pragma once

class Object
{
public:
    enum ObjectType
    {
        objUnknown = 0,
        objPlayer,
        objVessel,
        objCelestialStar,
        objCelestialBody,
    };

    Object(const cstr_t &name, ObjectType type)
    : objType(type)
    {
        objNames[0] = name;
    }

    inline double getRadius() const { return radius; }

private:
    ObjectType objType = objUnknown;
    std::vector<str_t> objNames{1};

protected:
    vec3d_t objPosition = { 0, 0, 0 };
    vec3d_t objVelocity = { 0, 0, 0 };
    quatd_t objRotation = { 1, 0, 0, 0 };

    double  mass   = 0.0;
    double  radius = 0.0;
    double  albedo = 0.0;
};