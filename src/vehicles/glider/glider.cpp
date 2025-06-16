// glider.cpp - Space glider vehicle module padcage
//
// Author:  Tim Stark
// Date:    Jun 11, 2025

#include "main/core.h"
#include "api/logger.h"
#include "api/module.h"
#include "glider.h"

Logger *sgLogger = nullptr;

Glider::Glider(Vehicle *vehicle, int flightModel)
: VehicleModule(vehicle)
{
    sgLogger = new Logger(Logger::logDebug, "glider.log");
    sgLogger->info("--------- Space Glider --------\n");

}

Glider::~Glider()
{

}
