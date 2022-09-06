// timedate.h - Time/Date package
//
// Author:  Tim Stark
// Date:    Aug 25, 20222

#pragma once

class TimeDate
{
public:
    TimeDate() = default;

    inline double getSysTime0() const       { return syst0; }
    inline double getSysTime1() const       { return syst1; }
    inline double getSysDeltaTime() const   { return sysdt; }

    inline double getSimTime0() const       { return simt0; }
    inline double getSimTime1() const       { return simt1; }
    inline double getSimDeltaTime0() const  { return simdt0; }
    inline double getSimDeltaTime1() const  { return simdt1; }

    inline double getMJD0() const           { return mjd0; }
    inline double getMJD1() const           { return mjd1; }
    inline double getJD0() const            { return jd0; }
    inline double getJD1() const            { return jd1; }

    inline double getFPS() const            { return fps; }

    void reset(double mjd = 0.0);

    void beginStep(double dt, bool bRunning);
    void endStep(bool bRunning);

private:

    double syst0, syst1;    // system time since system started [s]
    double sysdt;           // system delta time [s]

    double timeWarp = 1.0;

    double simt0, simt1;    // Simulation time since simulation time started [s]
    double simdt0, simdt1;  // Simulation delta time [s]

    double mjdref;          // MJD referene at starting time [days]
    double mjd0, mjd1;      // Modified Julian Time [days]
    double jd0, jd1;        // Julian time [days]

    size_t frameCount;
    size_t frameTick;
    size_t sysTick;     // Time index for FPS calculation
    double systa;       // Accumulated system time for FPS calculation
    double fps;         // Frame per second rate
};