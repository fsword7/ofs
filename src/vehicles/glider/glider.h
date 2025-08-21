// module.h - Space Plane 1 Vessel module package
//
// Author:  Tim Stark
// Date:    Mar 10, 2024

#include "api/logger.h"
#include "api/module.h"
#include "api/vehicle.h"
#include "engine/vehicle/vehicle.h"

class Glider : public VehicleModule
{
public:
    Glider(Vehicle *vehicle, int flightModel = 0);
    ~Glider();

    void finalizePostCreation() override;
    void setClassCaps() override;
    void setGearParameters(double state) override;

protected:
    bool bGearDown = false;

    tank_t *phMain = nullptr;
    tank_t *phAtt  = nullptr;

    // Individual thrusters
    thrust_t *thMain[2];
    thrust_t *thRetro[2];
    thrust_t *thHover[3];
    thrust_t *thAttRot[12];
    thrust_t *thAttLin[10];

    // Main/Retro/Hover thrusters group
    thrustgrp_t *thgrpMain;
    thrustgrp_t *thgrpRetro;
    thrustgrp_t *thgrpHover;

    // Rotation attitude thrusters group
    thrustgrp_t *thgrpPitchUp;
    thrustgrp_t *thgrpPitchDown;
    thrustgrp_t *thgrpYawLeft;
    thrustgrp_t *thgrpYawRight;
    thrustgrp_t *thgrpBankLeft;
    thrustgrp_t *thgrpBankRight;

    // Linear attitude thrusters group
    thrustgrp_t *thgrpMoveUp;
    thrustgrp_t *thgrpMoveDown;
    thrustgrp_t *thgrpMoveLeft;
    thrustgrp_t *thgrpMoveRight;
    thrustgrp_t *thgrpMoveForward;
    thrustgrp_t *thgrpMoveBackward;
};

extern Logger *sgLogger;