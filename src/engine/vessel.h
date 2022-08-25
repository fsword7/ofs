// vessel.h - Vessel (spacecraft) package
//
// Author:  Tim Stark
// Date:    Apr 24, 2022

#pragma once

#include "engine/rigidbody.h"

// class SuperVessel
// {
// public:
//     SuperVessel() = default;
//     ~SuperVessel() = default;


// };

// class Vessel
// {
// public:
//     Vessel() = default;
//     ~Vessel() = default;


// };

class SuperVessel;

// planetary surface parameters for flight simulation (air flight)
struct surface_t
{
    void setLanded(double lng, double lat, double alt, double dir,
        const vec3d_t &nml, const Object *object);

    const Object *body;     // celestial body reference

    // ship parameters for flight simulation
    double  lng, lat, rad;  // ship's position in planet's
                            // local equatorial coordinates
    double  clng, slng;     // cosines and sines of
    double  clat, slat;     //   longtidude and latitude
    double  airSpeed;       // air speed relative to wind
    double  groundSpeed;    // ground speed relative to ground
    double  altitude;       // ship's altitude above ground
    double  altitude0;      // ship's altitude above mean radius
    double  pitch, bank;    // ship orientation in horizon frame
    double  heading;        // compass orientation [radians]

    // ground parameters
    vec3d_t surfNormal;     // surface normal in local horizon frame
    double  elev;           // ground elevation to relative mean radius

    // atomsphere parameters
    bool    inAtomsphere;   // ship within planetary atomsphere
    double  atmPressure;    // atomsphere pressure [Pa]
    double  atmDensity;     // atomsphere density [Kg/m^3]
    double  atmTemp;        // atomsphere temperature [K]
    double  atmMach;        // atomsphere mach number

    mat3d_t l2h;            // planet to local horizon [planet frame]
};

enum AirfoilType_t
{
    liftVertical,           // lift is vetical (elevator, aileron, etc)
    liftHorizontal          // lift is horizationtal (rudder, etc)
};

enum ControlType_t
{
    aircAileron,            // Aileron control (bank control)
    aircFlaps,              // Flaps control (lift control)
    aircElevator,           // Elevator control (pitch)
    aircRudder,             // Rudder control 
    aircElevatorTrim,       // Elevtor trim control
    aircRudderTrim          // Rudder trim control
};

struct airfoil_t
{
    AirfoilType_t align;    // Vetical/Honzontal type
    double c;               // airfoil chord length;
    double S;               // reference area (wing)
    double A;               // aspect ratio (b^2/S with wingspan b)
};

struct tank_t
{
    double maxMass;         // maximum propellant mass
    double mass;            // current propellant mass
    double efficiency;      // fuel efficiency factor
};

struct thruster_t
{
    vec3d_t pos;            // thruster position in vessel frame
    vec3d_t dir;            // thrister direction

    tank_t  *tank;          // propellant resources
};

enum thrustType_t
{
    thrustMain,             // main thruster
    thrustRetro,            // retro thruster
    thrustHover             // hover thruster
};

// touchdown points for collision detection
struct tdVertex_t
{
    vec3d_t pos;            // position in vessel frame
    double  stiffness;      // suspension - stiffness [ N/m ] 
    double  damping;        // suspension - damping
    double  compression;    // suspension - compression factor
};

class VesselBase : public RigidBody
{
public:
    enum FlightStatus
    {
        fsFlight,   // in flight
        fsDriving,  // driving on ground
        fsLanded,   // Landed/All stopped
        fsCrashed   // Crashed
    };

    VesselBase();
    virtual ~VesselBase() = default;

    virtual void getIntermediateMoments(const StateVectors &state, vec3d_t &acc, vec3d_t &am,  double dt);
    virtual bool addSurfaceForces(const StateVectors &state, vec3d_t &acc, vec3d_t &am, double dt);

protected:
    FlightStatus fsType = fsFlight;

    surface_t surfParam;    // ship parameters in planet atomsphere
    mat3d_t   landrot;      // Ship orientation in local planet frame

    vec3d_t F;      // Linear moment
    vec3d_t L;      // Angular moment
    vec3d_t Fadd;   // collecting linear forces
    vec3d_t Ladd;   // collecting torque components

};

class Vessel : public VesselBase
{
    friend class SuperVessel;

public:
    Vessel();
    virtual ~Vessel() = default;

    inline void clearTouchdownPoints()          { tpVertices.clear(); }

    void initLanded(Object *object, double lat, double lng, double dir);
    void initDocked();
    void initOrbiting();

    void getIntermediateMoments(const StateVectors &state, vec3d_t &acc, vec3d_t &am,  double dt) override;
    bool addSurfaceForces(const StateVectors &state, vec3d_t &acc, vec3d_t &am,  double dt) override;
    
    void setGenericDefaults();
    void setTouchdownPoints(const tdVertex_t *tdvtx, int ntd);

    void updateMass();
    void update();

private:
    SuperVessel *superVessel = nullptr;

    double  lift;   // Lift force from wings
    double  drag;   // drag from atomspheric forces
    
    double  emass, fmass, pfmass; // empty mass, fuel mass, and previous fuel mass

    // Collision detection parameters (touchdown points)
    std::vector<tdVertex_t> tpVertices; // touchdown vertices (vessel frame)
    vec3d_t tpNormal;                   // upward normal of touchdown plane (vessel frame)
    vec3d_t tpCGravity;                 // center of gravity projection
    double  cogElev;

};
