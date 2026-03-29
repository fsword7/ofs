// object.h - Object package
//
// Author:  Tim Stark
// Date:    Apr 15, 2022

#define OPSAPI_SERVER_BUILD

#include "main/core.h"
#include "engine/object.h"
#include "utils/json.h"

Object::Object(const cstr_t &name, ObjectType type)
: objType(type)
{
    objNames[0] = name;

    initStateVectors();
}

Object::Object(cjson &config, ObjectType type)
: objType(type)
{
    initStateVectors();

    if (objType == objCelestialBody || objType == objCelestialStar)
    {
        setsName(myjson::getString<str_t>(config, "name", "(unknown)"));
        setMass(myjson::getFloat<double>(config, "mass"));
        setRadius(myjson::getFloat<double>(config, "radius"));
        setAlbedo(myjson::getFloat<double>(config, "geom-albedo"));
    }
}

void Object::update(bool force)
{
}

void Object::beginUpdate()
{
    // ofsLogger->info("{}: Begin updates\n", getsName());

    // Enable s1 updates
    s1 = (s0 == sv) ? sv+1 : sv;
}

void Object::endUpdate()
{
    // ofsLogger->info("{}: End updates - {},{},{}\n",
    //     getsName(), s1.pos.x, s1.pos.y, s1.pos.z);

    // s0.bUpdates = false;    // All done - clear updates flag
    // s0 = s1;                // Move all to S0 for on the air
    // s1 = {};                // Clear all for next update

    // // Set identity for R and Q rotations
    // s1.R = glm::identity<glm::dmat3>();
    // // s1.Q = glm::identity<glm::dquat>();
    // s1.Q = s1.R;

    // swap state vectprs amd disable s1 updates.
    flipStateVectors();
    s1 = nullptr;

    // ofsLogger->debug("{}: Q = ({},{},{},{})\n",
    //     getsName(), s1.Q.w, s1.Q.x, s1.Q.y, s1.Q.z);
    // ofsLogger->debug("{}: R = {},{},{}\n",
    //     getsName(), s1.R[0][0], s1.R[0][1], s1.R[0][2]);
    // ofsLogger->debug("{}:     {},{},{}\n",
    //     getsName(), s1.R[1][0], s1.R[1][1], s1.R[1][2]);
    // ofsLogger->debug("{}:     {},{},{}\n",
    //     getsName(), s1.R[2][0], s1.R[2][1], s1.R[2][2]);
}

void Object::initStateVectors()
{
    // Initialize state vectors;
    sv[0].pos = {};
    sv[0].vel = {};
    sv[0].omega = {};
    sv[0].R = glm::identity<glm::dmat3>();
    sv[0].Q = sv[0].R;
    sv[1] = sv[0];

    // Initialize double-buffer state vectors
    extern bool ofsStateUpdate;
    s0 = sv;
    s1 = ofsStateUpdate ? sv+1 : sv;
}

void Object::flipStateVectors()
{
    // Flip state vectors front and back as double buffers. 
    if (s0 == sv)
        s0 = sv+1, s1 = sv;
    else
        s0 = sv, s1 = sv+1;
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