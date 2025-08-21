// glider.cpp - Space glider vehicle module padcage
//
// Author:  Tim Stark
// Date:    Jun 11, 2025

#include "main/core.h"
#include "api/logger.h"
#include "api/module.h"
#include "engine/vehicle/vehicle.h"
#include "glider.h"

// Temp - Borrowed touchdown points from Delta Glider to test.
// New touchdown points will be made for this glider later.
static const tdVertex_t tdvtxGearDown[] = {
    // wheels - gear down
    {{  0,    -2.57, 10    }, 1e6, 1e5, 1.6, 0.1},
    {{ -3.5,  -2.57, -1    }, 1e6, 1e5, 3.0, 0.2},
    {{  3.5,  -2.57, -1    }, 1e6, 1e5, 3.0, 0.2},

    // vehicle frame
    {{ -8.5,  -0.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  8.5,  -0.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{ -8.5,  -0.4,  -3    }, 1e7, 1e5, 3.0},
    {{  8.5,  -0.4,  -3    }, 1e7, 1e5, 3.0},
    {{ -8.85,  2.3,  -5.05 }, 1e7, 1e5, 3.0},
    {{  8.85,  2.3,  -5.05 }, 1e7, 1e5, 3.0},
    {{ -8.85,  2.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  8.85,  2.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  0,     2,     6.2  }, 1e7, 1e5, 3.0},
    {{  0,    -0.6,  10.65 }, 1e7, 1e5, 3.0}
};

static const tdVertex_t tdvtxGearUp[] = {
    // wheels - gear up
    {{  0,    -1.5,   9    }, 1e7, 1e5, 3.0, 3.0},
    {{ -6,    -0.8,  -5    }, 1e7, 1e5, 3.0, 3.0},
    {{  3,    -1.2,  -5    }, 1e7, 1e5, 3.0, 3.0},

    // vehicle frame
    {{ -8.5,  -0.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  8.5,  -0.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{ -8.5,  -0.4,  -3    }, 1e7, 1e5, 3.0},
    {{  8.5,  -0.4,  -3    }, 1e7, 1e5, 3.0},
    {{ -8.85,  2.3,  -5.05 }, 1e7, 1e5, 3.0},
    {{  8.85,  2.3,  -5.05 }, 1e7, 1e5, 3.0},
    {{ -8.85,  2.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  8.85,  2.3,  -7.05 }, 1e7, 1e5, 3.0},
    {{  0,     2,     6.2  }, 1e7, 1e5, 3.0},
    {{  0,    -0.6,  10.65 }, 1e7, 1e5, 3.0}
};

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

void Glider::finalizePostCreation()
{
    setGearParameters(1.0);
}

void Glider::setClassCaps()
{
    sgLogger->info("Class capabilities initialzation\n");

    veh->setSize(10.0);
    veh->setMass(10000.0);

    // Propellant tanks
    phMain = veh->createPropellant(10400.0);
    phAtt = veh->createPropellant(600.0);
    veh->setDefaultPropellant(phMain);

    // Main engine thrusters
    thMain[0] = veh->createThruster({-1, 0, -7}, {0, 0, 1}, 2e6, phMain);
    thMain[1] = veh->createThruster({ 1, 0, -7}, {0, 0, 1}, 2e6, phMain);
    thgrpMain = veh->createThrusterGroup(thMain, 2, thgMain);

    // Retro engine thrusters
    thRetro[0] = veh->createThruster({-3, 0, 5}, {0, 0, -1}, 3.4e4, phMain);
    thRetro[1] = veh->createThruster({ 3, 0, 5}, {0, 0, -1}, 3.4e4, phMain);
    thgrpRetro = veh->createThrusterGroup(thRetro, 2, thgRetro);

    // Hover engine thrusters
    thHover[0] = veh->createThruster({ 0, 0,  3}, {0, 1, 0}, 1.4e5, phMain);
    thHover[1] = veh->createThruster({-3, 0, -4}, {0, 1, 0}, 3/4*0.5*1.4e5, phMain);
    thHover[2] = veh->createThruster({ 3, 0, -4}, {0, 1, 0}, 3/4*0.5*1.4e5, phMain);
    thgrpHover = veh->createThrusterGroup(thHover, 3, thgHover);

    // Rotation/linear attitude thrusters
    thAttRot[0] = thAttLin[0] = veh->createThruster({ 0, 0, 8}, { 0, 1, 0}, 2.5e3, phAtt);
    thAttRot[1] = thAttLin[3] = veh->createThruster({ 0, 0,-8}, { 0,-1, 0}, 2.5e3, phAtt);
    thAttRot[2] = thAttLin[2] = veh->createThruster({ 0, 0, 8}, { 0,-1, 0}, 2.5e3, phAtt);
    thAttRot[3] = thAttLin[1] = veh->createThruster({ 0, 0,-8}, { 0, 1, 0}, 2.5e3, phAtt);

    thgrpPitchUp = veh->createThrusterGroup(thAttRot, 2, thgRotPitchUp);
    thgrpPitchDown = veh->createThrusterGroup(thAttRot+2, 2, thgRotPitchDown);
    thgrpMoveUp = veh->createThrusterGroup(thAttLin, 2, thgLinMoveUp);
    thgrpMoveDown = veh->createThrusterGroup(thAttLin+2, 2, thgLinMoveDown);


    thAttRot[4] = thAttLin[4] = veh->createThruster({ 0, 0, 6}, {-1, 0, 0}, 2.5e3, phAtt);
    thAttRot[5] = thAttLin[7] = veh->createThruster({ 0, 0,-6}, { 1, 0, 0}, 2.5e3, phAtt);
    thAttRot[6] = thAttLin[6] = veh->createThruster({ 0, 0, 6}, { 1, 0, 0}, 2.5e3, phAtt);
    thAttRot[7] = thAttLin[5] = veh->createThruster({ 0, 0,-6}, {-1, 0, 0}, 2.5e3, phAtt);

    thgrpYawLeft = veh->createThrusterGroup(thAttRot+4, 2, thgRotYawLeft);
    thgrpYawRight = veh->createThrusterGroup(thAttRot+6, 2, thgRotYawRight);
    thgrpMoveLeft = veh->createThrusterGroup(thAttLin+4, 2, thgLinMoveLeft);
    thgrpMoveRight = veh->createThrusterGroup(thAttLin+6, 2, thgLinMoveRight);


    thAttRot[8]  = veh->createThruster({ 6, 0, 0}, { 0, 1, 0}, 2.5e3, phAtt);
    thAttRot[9]  = veh->createThruster({-6, 0, 0}, { 0,-1, 0}, 2.5e3, phAtt);
    thAttRot[10] = veh->createThruster({-6, 0, 0}, { 0, 1, 0}, 2.5e3, phAtt);
    thAttRot[11] = veh->createThruster({ 6, 0, 0}, { 0,-1, 0}, 2.5e3, phAtt);

    thgrpBankLeft = veh->createThrusterGroup(thAttRot+8, 2, thgRotBankLeft);
    thgrpBankRight = veh->createThrusterGroup(thAttRot+10, 2, thgRotBankRight);


    thAttLin[8]  = veh->createThruster({ 0, 0,-7}, { 0, 0, 1}, 2.5e3, phAtt);
    thAttLin[9]  = veh->createThruster({ 0, 0, 7}, { 0, 0,-1}, 2.5e3, phAtt);

    thgrpMoveForward = veh->createThrusterGroup(thAttLin+8, 1, thgLinMoveForward);
    thgrpMoveBackward = veh->createThrusterGroup(thAttLin+9, 1, thgLinMoveBackward);



}

void Glider::setGearParameters(double state)
{
    if (state == 1.0) {
        if (!bGearDown) {
            veh->setTouchdownPoints(tdvtxGearDown, ARRAY_SIZE(tdvtxGearDown));
            veh->enableWheelSteering(true);
            bGearDown = true;
        }
    } else {
        if (bGearDown) {
            veh->setTouchdownPoints(tdvtxGearUp, ARRAY_SIZE(tdvtxGearUp));
            veh->enableWheelSteering(false);
            bGearDown = false;
        }
    }
}