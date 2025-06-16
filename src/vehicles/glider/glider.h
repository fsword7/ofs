// module.h - Space Plane 1 Vessel module package
//
// Author:  Tim Stark
// Date:    Mar 10, 2024

#include "api/logger.h"
#include "api/module.h"
#include "api/vehicle.h"


class Glider : public VehicleModule
{
public:
    Glider(Vehicle *vehicle, int flightModel = 0);
    ~Glider();

protected:
};

extern Logger *sgLogger;