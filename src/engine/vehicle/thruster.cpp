// thruster.cpp - Vehicle package - thrusters
//
// Author:  Tim Stark
// Date:    Oct 23, 2024

#include "main/core.h"
#include "engine/vehicle/vehicle.h"

tank_t *Vehicle::createPropellant(double maxMass, double mass, double efficiency)
{
    tank_t *ts = new tank_t;
    ts->maxMass = maxMass;
    ts->mass = (mass >= 0.0 ? mass : maxMass);
    ts->pmass = ts->mass;
    ts->efficiency = efficiency;

    if (tankList.empty())
        dTank = ts;
    tankList.push_back(ts);

    return ts;
}

thrust_t *Vehicle::createThruster(const glm::dvec3 &pos, const glm::dvec3 &dir, double maxth, tank_t *tank)
{
    thrust_t *th = new thrust_t();

    th->pos = pos / M_PER_KM;
    th->dir = dir;
    th->maxth = maxth;
    th->tank = tank;

    thrustList.push_back(th);

    return th;
}

bool Vehicle::deleteThruster(thrust_t *th)
{
    
    for (auto it = thrustList.begin(); it != thrustList.end(); it++)
    {
        if (*it == th)
        {
            it = thrustList.erase(it);
            return true;
        }
    }

    return false;
}

// void Vehicle::setThrustLevel(thrust_t *th, double level)
// {
//     double dlevel = level - th->lvperm;
//     th->lvperm = level;
//     if (th->tank != nullptr && th->tank->mass > 0)
//         th->level = std::max(0.0, std::min(1.0, th->level + dlevel));
// }

// void Vehicle::adjustThrustLevel(thrust_t *th, double dlevel)
// {
//     th->lvperm += dlevel;
//     if (th->tank != nullptr && th->tank->mass > 0)
//         th->level = std::max(0.0, std::min(1.0, th->level + dlevel));
// }

// void Vehicle::setThrustOverride(thrust_t *th, double level)
// {
//     th->lvover = level;
// }

// void Vehicle::adjustThrustOverride(thrust_t *th, double dlevel)
// {
//     th->lvover += dlevel;
// }

// Thruster Group

thrustgrp_t *Vehicle::createThrusterGroup(thrust_t **th, int nThrusts, thrustType_t type)
{
    thrustgrp_t *thg = new thrustgrp_t();

    for (int idx = 0; idx < nThrusts; idx++)
        thg->thrusters.push_back(th[idx]);

    thgrpList[type] = thg;
    return thg;
}

void Vehicle::createDefaultEngine(thrustType_t type, double power)
{
    switch (type)
    {
    case thgMain:
        break;
    case thgRetro:
        break;
    case thgHover:
        break;
    }
}

void Vehicle::createDefaultAttitudeSet(double maxth)
{
    tank_t *tank = /* tanksList.size() > 0 ? tanksList[0] : */ nullptr;
    thrust_t *thlin, *throt[2];

    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3( 0, 1, 0), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveUp);
    
    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3( 0,-1, 0), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveDown);
    
    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3(-1, 0, 0), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveLeft);
    
    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3( 1, 0, 0), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveRight);
    
    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3( 0, 0,-1), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveForward);
    
    thlin = createThruster(glm::dvec3(0,0,0), glm::dvec3( 0, 0, 1), maxth);
    createThrusterGroup(&thlin, 1, thgLinMoveBackward);

    double size = maxth * 0.5;

    throt[0] = createThruster(glm::dvec3(0,0,-size), glm::dvec3( 0, 1, 0), maxth);
    throt[1] = createThruster(glm::dvec3(0,0, size), glm::dvec3( 0,-1, 0), maxth);
    createThrusterGroup(throt, 2, thgRotPitchUp);

    throt[0] = createThruster(glm::dvec3(0,0,-size), glm::dvec3( 0,-1, 0), maxth);
    throt[1] = createThruster(glm::dvec3(0,0, size), glm::dvec3( 0, 1, 0), maxth);
    createThrusterGroup(throt, 2, thgRotPitchDown);
    
    throt[0] = createThruster(glm::dvec3( size,0,0), glm::dvec3( 0, 0,-1), maxth);
    throt[1] = createThruster(glm::dvec3(-size,0,0), glm::dvec3( 0, 0, 1), maxth);
    createThrusterGroup(throt, 2, thgRotYawLeft);
    
    throt[0] = createThruster(glm::dvec3( size,0,0), glm::dvec3( 0, 0, 1), maxth);
    throt[1] = createThruster(glm::dvec3(-size,0,0), glm::dvec3( 0, 0,-1), maxth);
    createThrusterGroup(throt, 2, thgRotYawRight);
    
    throt[0] = createThruster(glm::dvec3( size,0,0), glm::dvec3( 0, 1, 0), maxth);
    throt[1] = createThruster(glm::dvec3(-size,0,0), glm::dvec3( 0,-1, 0), maxth);
    createThrusterGroup(throt, 2, thgRotBankLeft);  
    
    throt[0] = createThruster(glm::dvec3( size,0,0), glm::dvec3( 0,-1, 0), maxth);
    throt[1] = createThruster(glm::dvec3(-size,0,0), glm::dvec3( 0, 1, 0), maxth);
    createThrusterGroup(throt, 2, thgRotBankRight);

}


void Vehicle::setThrustGroupLevel(thrustgrp_t *tg, double level)
{
    for (auto th : tg->thrusters)
        th->setThrustLevel(level);
}

void Vehicle::throttleThrustGroupLevel(thrustgrp_t *tg, double dlevel)
{
    for (auto th : tg->thrusters)
        th->adjustThrustLevel(dlevel);
}

void Vehicle::setThrustGroupOverride(thrustgrp_t *tg, double level)
{
    for (auto th : tg->thrusters)
        th->setThrustOverride(level);
}

void Vehicle::throttleThrustGroupOverride(thrustgrp_t *tg, double dlevel)
{
    for (auto th : tg->thrusters)
        th->adjustThrustOverride(dlevel);
}


void Vehicle::setThrustGroupLevel(thrustType_t type, double level)
{
    if (thgrpList[type] != nullptr)
        setThrustGroupLevel(thgrpList[type], level);
}

void Vehicle::throttleThrustGroupLevel(thrustType_t type, double dlevel)
{
    if (thgrpList[type] != nullptr)
        throttleThrustGroupLevel(thgrpList[type], dlevel);
}

void Vehicle::setThrustGroupOverride(thrustType_t type, double level)
{
    if (thgrpList[type] != nullptr)
        setThrustGroupOverride(thgrpList[type], level);
}

void Vehicle::throttleThrustGroupOverride(thrustType_t type, double dlevel)
{
    if (thgrpList[type] != nullptr)
        throttleThrustGroupOverride(thgrpList[type], dlevel);
}


double Vehicle::getThrustGroupLevel(thrustgrp_t *tg)
{
    double level = 0.0;
    for (auto th : tg->thrusters)
        level += th->level;
    return tg->thrusters.size() > 0 ? level / tg->thrusters.size() : 0.0;
}

double Vehicle::getThrustGroupLevel(thrustType_t type)
{
    return (thgrpList[type] != nullptr)
        ? getThrustGroupLevel(thgrpList[type]) : 0.0;
}

void Vehicle::throttleMainRetroThruster(double dlevel)
{
    double thMain = getThrustGroupLevel(thgMain);
    double thRetro = getThrustGroupLevel(thgRetro);

    if (dlevel > 0.0) {
         if (thRetro == 0.0) {
            setThrustGroupLevel(thgMain, std::min(thMain+dlevel, 1.0));
        } else {
            setThrustGroupLevel(thgRetro, std::max(thRetro-dlevel, 0.0));
            setThrustGroupLevel(thgMain, 0.0);
        }
    } else if (dlevel < 0.0) {
        if (thMain == 0.0) {
            setThrustGroupLevel(thgRetro, std::min(thRetro-dlevel, 1.0));
        } else {
            setThrustGroupLevel(thgMain, std::max(thMain+dlevel, 0.0));
            setThrustGroupLevel(thgRetro, 0.0);
        }
    }
}

void Vehicle::overrideMainRetroThruster(double level)
{
    if (level > 0.0) {
        setThrustGroupOverride(thgMain, level);
        setThrustGroupOverride(thgRetro, -1.0);
    } else {
        setThrustGroupOverride(thgMain, -1.0);
        setThrustGroupOverride(thgRetro, -level);
    }
}

void Vehicle::updateUserAttitudeControls(int *ctrlKeyboard)
{

    // Main engine controls
    throttleThrustGroupOverride(thgMain,   0.001 * ctrlKeyboard[thgMain]);
    throttleThrustGroupOverride(thgRetro,  0.001 * ctrlKeyboard[thgRetro]);
    throttleThrustGroupOverride(thgHover,  0.001 * ctrlKeyboard[thgHover]);
 
    if (rcsMode & 1)
    {
        // RCS Attitude rotational controls
        throttleThrustGroupOverride(thgRotPitchUp,      0.001 * ctrlKeyboard[thgRotPitchUp]);
        throttleThrustGroupOverride(thgRotPitchDown,    0.001 * ctrlKeyboard[thgRotPitchDown]);
        throttleThrustGroupOverride(thgRotYawLeft,      0.001 * ctrlKeyboard[thgRotYawLeft]);
        throttleThrustGroupOverride(thgRotYawRight,     0.001 * ctrlKeyboard[thgRotYawRight]);
        throttleThrustGroupOverride(thgRotBankLeft,     0.001 * ctrlKeyboard[thgRotBankLeft]);
        throttleThrustGroupOverride(thgRotBankRight,    0.001 * ctrlKeyboard[thgRotBankRight]);
    }

    if (rcsMode & 2) 
    {
        // RCS Attitude linear controls
        throttleThrustGroupOverride(thgLinMoveUp,       0.001 * ctrlKeyboard[thgLinMoveUp]);
        throttleThrustGroupOverride(thgLinMoveDown,     0.001 * ctrlKeyboard[thgLinMoveDown]);
        throttleThrustGroupOverride(thgLinMoveLeft,     0.001 * ctrlKeyboard[thgLinMoveLeft]);
        throttleThrustGroupOverride(thgLinMoveRight,    0.001 * ctrlKeyboard[thgLinMoveRight]);
        throttleThrustGroupOverride(thgLinMoveForward,  0.001 * ctrlKeyboard[thgLinMoveForward]);
        throttleThrustGroupOverride(thgLinMoveBackward, 0.001 * ctrlKeyboard[thgLinMoveBackward]);
    }
}

void Vehicle::updateThrustForces()
{

    glm::dvec3 thrust = { 0, 0, 0 };
    glm::dvec3 F;

    for (auto th : thrustList)
    {
        th->level = std::max(0.0, std::min(1.0, th->lvperm + th->lvover));
        if (th->level > 0.0)
        {   
            tank_t *tank = th->tank;

            double th0 = th->level * th->maxth;
            F = th->dir * th0;
            thrust += F;
            camom += glm::cross(F, th->pos);
        }
        th->lvover = 0.0;
    }

    cflin += thrust;
}
