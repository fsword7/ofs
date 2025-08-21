// vehicle.h - Vehicle API package for vehicle modules
//
// Author:  Tim Stark
// Date:    Mar 9, 2024

#pragma once

#include "api/ofsapi.h"
// #include "engine/vehicle/vehicle.h"

class Vehicle;

enum thrustType_t
{
    thgMain = 0,            // main thruster
    thgRetro,               // retro thruster
    thgHover,               // hover thruster

    thgRotPitchUp,          // Rotation Attitude: Rotate pitch up
    thgRotPitchDown,        // Rotation Attitude: Rotate pitch down
    thgRotYawLeft,          // Rotation Attitude: Rotate yaw left
    thgRotYawRight,         // Rotation Attitude: Rotate yaw right
    thgRotBankLeft,         // Rotation Attitude: Rotate bank left
    thgRotBankRight,        // Rotation Attitude: Rotate bank right
    
    thgLinMoveUp,           // Linear Attitude: Move up
    thgLinMoveDown,         // Linear Attitude: Move down
    thgLinMoveLeft,         // Linear Attitude: Move Left
    thgLinMoveRight,        // Linear Attitude: Move right
    thgLinMoveForward,      // Linear Attitude: Move forward
    thgLinMoveBackward,     // Linear Attitude: Move backward

    thgMaxThrusters,

    thgUser = 0x80          // User-definable thrusters
};

struct thrust_t;
struct tank_t;

class OFSAPI VehicleModule
{
public:
    VehicleModule(Vehicle *vehicle)
    : veh(vehicle)
    { }

    virtual void finalizePostCreation() {}
    virtual void setClassCaps() {}
    virtual void setGearParameters(double state) {}

protected:
    // thrust_t *createThruster(const glm::dvec3 &pos, const glm::dvec3 &dir, double maxth, tank_t *tank = nullptr);
    // void createThrusterGroup(thrust_t **th, int nThrusts, thrustType_t type);

protected:
    Vehicle *veh = nullptr;
};
