// glider.cpp - Space glider vehicle module padcage
//
// Author:  Tim Stark
// Date:    Jun 11, 2025

#include "main/core.h"
#include "api/vehicle.h"
#include "glider.h"

Logger *sgLogger = nullptr;

LIBCALL VehicleModule *ovcInit(Vehicle *vehicle)
{
    // sgLogger = new Logger(Logger::logDebug, "glider.log");
    // sgLogger->info("--------- Space Glider --------\n");

    return nullptr;
}

LIBCALL void ovcExit(Vehicle *vehicle)
{
}

