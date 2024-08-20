// timedate.cpp - Time/Date package
//
// Author:  Tim Stark
// Date:    Aug 25, 20222

#include "main/core.h"
#include "main/timedate.h"
#include "universe/astro.h"

void TimeDate::reset(double t)
{
    // Reset system/simulation time
    syst0 = syst1 = sysdt = 0.0;
    simt0 = simt1 = simdt0 = simdt1 = 0.0;

    // Reset FPS rate
    frameCount = 0;
    frameTick = 0;
    sysTick = 0;
    fps = systa = 0.0;

    // Reset julian dates
    mjdref = astro::MJD(t);
    mjd0 = mjd1 = mjdref;
    // jd0 = jd1 = mjdref;
}

void TimeDate::beginStep(double dt, bool running)
{
    // Update system time
    sysdt = dt;
    syst1 = syst0 + sysdt;

    // Update simulation time if running enabled
    if (running)
    {
        simdt1 = sysdt * timeWarp;
        simt1 = simt0 + simdt1;
        mjd1 = mjdref + astro::Day(simt1);
        // jd1 = jdref + astro::Day(simt1);
        // Logger::getLogger()->info("MJD Time: {} => {}\n",
        //     mjd1, astro::getMJDDateStr(mjd1));
    }

    // count frame/FPS update
    frameCount++;
    frameTick++;
    systa += sysdt;
    if ((size_t)syst1 != sysTick)
    {
        fps = frameTick / systa;
        frameTick = 0;
        systa = 0.0;
        sysTick = (size_t)syst1;
    }
}

void TimeDate::endStep(bool running)
{
    // Update values for "On The Air" display.
    syst0 = syst1;
    if (running)
    {
        simt0 = simt1;
        simdt0 = simdt1;
        mjd0 = mjd1;
        jd0 = jd1;
    }
}

void TimeDate::setTimeWarp(double twarp)
{
    timeWarp = twarp;
}

