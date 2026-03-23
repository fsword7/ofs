// thruster.cpp - Vehicle package - thrusters
//
// Author:  Tim Stark
// Date:    Oct 23, 2024

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "main/keymap.h"
#include "main/app.h"
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

thrust_t *Vehicle::createThruster(const glm::dvec3 &pos, const glm::dvec3 &dir, double maxth,
    tank_t *tank, double isp, double ispref, double pref)
{
    thrust_t *th = new thrust_t();

    th->pos = pos / M_PER_KM;
    th->dir = dir;
    th->maxth = maxth;
    th->tank = tank;

    // Atmospheric pressure dependency of thrust
    th->isp = isp; // (isp > 0.0) ? isp : disp;
    th->pfac = (ispref > 0.0) ? (isp-ispref)/(pref/isp) : 0.0;

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
    default:
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

void Vehicle::setMainRetroThruster(double level)
{
    if (level >= 0.0) {
        setThrustGroupLevel(thgMain, level);
        setThrustGroupLevel(thgRetro, 0.0);
    } else {
        setThrustGroupLevel(thgRetro, -level);
        setThrustGroupLevel(thgMain, 0.0);
    }
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

// Reaction Control System
// 0 = turned off
// 1 = rotation control
// 2 = linear control

void Vehicle::setRCSMode(int mode)
{
    if (rcsMode != mode) {
        rcsMode = mode;
        ofsLogger->debug("{}: Enable RCS Mode {}\n", getsName(), rcsMode);
    }
}

int Vehicle::toggleRCSMode()
{
    // Toggle RCS mode between rotation and linear controls
    if (rcsMode >= 1) {
        rcsMode ^= 3;
        ofsLogger->debug("{}: Toggle RCS Mode {}\n", getsName(), rcsMode);
    }
    return rcsMode;
}

bool Vehicle::processImmediateKeyOnRunning(const bool *keyState, const Keymap &keymap)
{
    double dt = ofsDate->getSysDeltaTime();

    // Clear all thruster controls
    for (int idx = 0; idx < thgMaxThrusters; idx++)
        ctrlKeyThrusters[idx] = 0;

    // RCS rotstion controls
    if (rcsMode == 1) {
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotPitchUp))            ctrlKeyThrusters[thgRotPitchUp]      = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotPitchUp))           ctrlKeyThrusters[thgRotPitchUp]      = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotPitchDown))          ctrlKeyThrusters[thgRotPitchDown]    = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotPitchDown))         ctrlKeyThrusters[thgRotPitchDown]    = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotYawLeft))            ctrlKeyThrusters[thgRotYawLeft]      = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotYawLeft))           ctrlKeyThrusters[thgRotYawLeft]      = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotYawRight))           ctrlKeyThrusters[thgRotYawRight]     = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotYawRight))          ctrlKeyThrusters[thgRotYawRight]     = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotBankLeft))           ctrlKeyThrusters[thgRotBankLeft]     = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotBankLeft))          ctrlKeyThrusters[thgRotBankLeft]     = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSRotBankRight))          ctrlKeyThrusters[thgRotBankRight]    = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSRotBankRight))         ctrlKeyThrusters[thgRotBankRight]    = 100;
    }

    // RCS linear controls
    if (rcsMode == 2) {
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveUp))             ctrlKeyThrusters[thgLinMoveUp]       = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveUp))            ctrlKeyThrusters[thgLinMoveUp]       = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveDown))           ctrlKeyThrusters[thgLinMoveDown]     = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveDown))          ctrlKeyThrusters[thgLinMoveDown]     = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveLeft))           ctrlKeyThrusters[thgLinMoveLeft]     = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveLeft))          ctrlKeyThrusters[thgLinMoveLeft]     = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveRight))          ctrlKeyThrusters[thgLinMoveRight]    = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveRight))         ctrlKeyThrusters[thgLinMoveRight]    = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveForward))        ctrlKeyThrusters[thgLinMoveForward]  = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveForward))       ctrlKeyThrusters[thgLinMoveForward]  = 100;
        if (keymap.isLogicalKey(keyState, ofs::lkeyRCSLinMoveBackward))       ctrlKeyThrusters[thgLinMoveBackward] = 1000;
        if (keymap.isLogicalKey(keyState, ofs::lkeyLRCSLinMoveBackward))      ctrlKeyThrusters[thgLinMoveBackward] = 100;
    }

    // Main/Retro thruster controls
    if (keymap.isLogicalKey(keyState, ofs::lkeyIncMainThrust))
        throttleMainRetroThruster(0.2*dt);
    if (keymap.isLogicalKey(keyState, ofs::lkeyDecMainThrust))
        throttleMainRetroThruster(-0.2*dt);
    if (keymap.isLogicalKey(keyState, ofs::lkeyFullMainThrust))
        setMainRetroThruster(1.0);
    if (keymap.isLogicalKey(keyState, ofs::lkeyFullRetroThrust))
        setMainRetroThruster(-1.0);
    if (keymap.isLogicalKey(keyState, ofs::lkeyBoostMainThrust))
        overrideMainRetroThruster(1.0);
    if (keymap.isLogicalKey(keyState, ofs::lkeyBoostRetroThrust))
        overrideMainRetroThruster(-1.0);
    if (keymap.isLogicalKey(keyState, ofs::lkeyKillMainThrust)) {
        setThrustGroupLevel(thgMain, 0.0);
        setThrustGroupLevel(thgRetro, 0.0);
    }

    // Hover thruster controls
    if (keymap.isLogicalKey(keyState, ofs::lkeyIncHoverThrust))
        throttleThrustGroupLevel(thgHover, 0.2*dt);
    if (keymap.isLogicalKey(keyState, ofs::lkeyDecHoverThrust))
        throttleThrustGroupLevel(thgHover, -0.2*dt);

    return false;
}

bool Vehicle::processBufferedKeyOnRunning(uint8_t key, const bool *keyState, const Keymap &keymap)
{
    if (keymap.isLogicalKey(key, keyState, ofs::lkeyEnableRCSMode))
        setRCSMode(rcsMode >= 1 ? 0 : 1);
    if (keymap.isLogicalKey(key, keyState, ofs::lkeyToggleRCSMode))
        toggleRCSMode();

    return false;
}

void Vehicle::updateUserAttitudeControls()
{

    // Main engine controls
    throttleThrustGroupOverride(thgMain,   0.001 * ctrlKeyThrusters[thgMain]);
    throttleThrustGroupOverride(thgRetro,  0.001 * ctrlKeyThrusters[thgRetro]);
    throttleThrustGroupOverride(thgHover,  0.001 * ctrlKeyThrusters[thgHover]);
 
    if (rcsMode & 1)
    {
        // RCS Attitude rotational controls
        throttleThrustGroupOverride(thgRotPitchUp,      0.001 * ctrlKeyThrusters[thgRotPitchUp]);
        throttleThrustGroupOverride(thgRotPitchDown,    0.001 * ctrlKeyThrusters[thgRotPitchDown]);
        throttleThrustGroupOverride(thgRotYawLeft,      0.001 * ctrlKeyThrusters[thgRotYawLeft]);
        throttleThrustGroupOverride(thgRotYawRight,     0.001 * ctrlKeyThrusters[thgRotYawRight]);
        throttleThrustGroupOverride(thgRotBankLeft,     0.001 * ctrlKeyThrusters[thgRotBankLeft]);
        throttleThrustGroupOverride(thgRotBankRight,    0.001 * ctrlKeyThrusters[thgRotBankRight]);
    }

    if (rcsMode & 2) 
    {
        // RCS Attitude linear controls
        throttleThrustGroupOverride(thgLinMoveUp,       0.001 * ctrlKeyThrusters[thgLinMoveUp]);
        throttleThrustGroupOverride(thgLinMoveDown,     0.001 * ctrlKeyThrusters[thgLinMoveDown]);
        throttleThrustGroupOverride(thgLinMoveLeft,     0.001 * ctrlKeyThrusters[thgLinMoveLeft]);
        throttleThrustGroupOverride(thgLinMoveRight,    0.001 * ctrlKeyThrusters[thgLinMoveRight]);
        throttleThrustGroupOverride(thgLinMoveForward,  0.001 * ctrlKeyThrusters[thgLinMoveForward]);
        throttleThrustGroupOverride(thgLinMoveBackward, 0.001 * ctrlKeyThrusters[thgLinMoveBackward]);
    }
}

void Vehicle::updateThrustForces()
{
    // Nagivation computer sequences
    if (navFlags) {
        // Kill rotation navigation control
        if (navFlags & NAVBIT_KILLROT) {
            
        }
    }

    // Record previous fuel mass
    for (auto &ts : tankList)
        ts->pmass = ts->mass;

    glm::dvec3 thrust = {};
    glm::dvec3 tamom = {};

    double dt = ofsDate->getSimDeltaTime1();
    bool bThrustEngaged = false;
    for (auto eng : thrustList) {
        eng->level = std::max(0.0, std::min(1.0, eng->lvperm + eng->lvover));
        tank_t *ts = eng->tank;
        if (ts != nullptr && ts->mass > 0.0) {
            double th = eng->maxth * eng->level;
            if (bEnableBurnFuel) {
                ts->mass -= th / (ts->efficiency * eng->isp) * dt;
                if (ts->mass < 0.0)
                    ts->mass = 0.0;
            }
            th *= computeAtmThrustScale(eng, surfParam.atmPressure);
            glm::dvec3 flin = eng->dir * th;
            thrust += flin;
            tamom += glm::cross(flin, eng->pos);
            bThrustEngaged = true;
        } else {
            // Run out of fuel so that
            // reset levels to zero.
            eng->level = 0.0;
            eng->lvperm = 0.0;
        }
        eng->lvover = 0.0;
    }

    if (bThrustEngaged)
        cflin += thrust;
    camom += tamom;

    ofsLogger->debug("{}: T Thrust ({},{},{}) Angular ({},{},{})\n",
        getsName(), thrust.x, thrust.y, thrust.z, tamom.x, tamom.y, tamom.z);
    // ofsLogger->debug("{}: T Thrust ({},{},{}) Angular ({},{},{})\n",
    //     getsName(), cflin.x, cflin.y, cflin.z, camom.x, camom.y, camom.z);
}
