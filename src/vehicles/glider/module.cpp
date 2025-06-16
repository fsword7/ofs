// glider.cpp - Space glider vehicle module padcage
//
// Author:  Tim Stark
// Date:    Jun 11, 2025

#include "main/core.h"
#include "api/vehicle.h"
#include "glider.h"

Glider *glider = nullptr;

LIBCALL VehicleModule *ovcInit(Vehicle *vehicle)
{
    glider = new Glider(vehicle, 0);
    return glider;
}

LIBCALL void ovcExit(VehicleModule *module)
{
    if (static_cast<Glider *>(module) == glider)
        delete glider;
}