// vehicle.h - Vehicle package
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

class VehicleModule;
class SuperVehicle;
class Vehicle;
class HUDPanel;
class Sketchpad;
class Mesh;

// planetary surface parameters for flight simulation (air flight)
struct surface_t
{
    void setLanded(double lng, double lat, double alt, double dir,
        const glm::dvec3 &nml, const Object *object);

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
    glm::dvec3 surfNormal;  // surface normal in local horizon frame
    double  elev;           // ground elevation to relative mean radius

    // atomsphere parameters
    bool    inAtomsphere;   // ship within planetary atomsphere
    double  atmPressure;    // atomsphere pressure [Pa]
    double  atmDensity;     // atomsphere density [Kg/m^3]
    double  atmTemp;        // atomsphere temperature [K]
    double  atmMach;        // atomsphere mach number

    glm::dmat3 l2h;            // planet to local horizon [planet frame]
};

using csurface_t = const surface_t;

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

struct thrust_t
{
    glm::dvec3 pos;            // thruster position in vessel frame
    glm::dvec3 dir;            // thrister direction

    double maxth;           // Maximum thrust force [N]
    double isp;
    double pfac;

    double level;           // level [0..1]
    double lvperm;          // level permament
    double lvover;          // level override

    tank_t  *tank;          // propellant resources

    inline void setThrustLevel(double lvl)
    {
        double dlevel = lvl - lvperm;
        lvperm = lvl;
        if (tank != nullptr && tank->mass > 0)
            level = std::max(0.0, std::min(1.0, level + dlevel));
    }

    inline void adjustThrustLevel(double dlvl)
    {
        lvperm += dlvl;
        if (tank != nullptr && tank->mass > 0)
            level = std::max(0.0, std::min(1.0, level + dlvl));
    }

    inline void setThrustOverride(double lvl)
    {
        lvover = lvl;
    }

    inline void adjustThrustOverride(double dlvl)
    {
        lvover += dlvl;
    }
};

using thlist_t = std::vector<thrust_t *>;

struct thrustgrp_t
{
    thlist_t thrusters;
    double sum_maxth;
};
using tglist_t = std::vector<thrustgrp_t *>;

enum thrustType_t
{
    thgMain = 0,            // main thruster
    thgRetro,               // retro thruster
    thgHover,               // hover thruster

    thgAttPitchUp,          // Attitude: Rotate pitch up
    thgAttPitchDown,        // Attitude: Rotate pitch down
    thgAttYawLeft,          // Attitude: Rotate yaw left
    thgAttYawRight,         // Attitude: Rotate yaw right
    thgAttBankLeft,         // Attitude: Rotate bank left
    thgAttBankRight,        // Attitude: Rotate bank right
    
    thgTraUp,               // Translation: Move up
    thgTraDown,             // Translation: Move down
    thgTraLeft,             // Translation: Move Left
    thgTraRight,            // Translation: Move right
    thgTraForward,          // Translation: Move forward
    thgTraBackward,         // Translation: Move backward

    thgMaxThrusters,

    thgUser = 0x80          // User-definable thrusters
};

// touchdown points for collision detection
struct tdVertex_t
{
    glm::dvec3 pos;            // position in vessel frame

    // Mass-spring-damper parameters
    double  stiffness;      // suspension - stiffness [ N/m ] 
    double  damping;        // suspension - damping
    double  compression;    // suspension - compression factor
};

// docking port definition
struct port_t
{
    glm::dvec3 port;    // docking port position
    glm::dvec3 dir;     // approaching direction
    glm::dvec3 rot;     // longitudional rotation alignment direction
};

typedef VehicleModule *(*ovcInit_t)(Vehicle *);
typedef void (*ovcExit_t)(VehicleModule *);

struct MeshEntry
{
    str_t meshName;
    glm::dvec3 meshOffset;
    bool isVisible;
    Mesh *mesh;
};
class VehicleBase : public RigidBody
{
public:
    enum FlightStatus
    {
        fsFlight,   // in flight
        fsDriving,  // driving on ground
        fsLanded,   // Landed/All stopped
        fsCrashed   // Crashed
    };

    VehicleBase(cstr_t &name);
    VehicleBase(YAML::Node &config);
    virtual ~VehicleBase() = default;

    inline surface_t *getSurfaceParameters() { return &surfParam; }
    inline csurface_t *getSurfaceParameters() const { return &surfParam; }
    
    // virtual void getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt);
    virtual bool addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt);

protected:
    FlightStatus fsType = fsFlight;

    surface_t surfParam;    // ship parameters in planet atomsphere
    glm::dmat3  landrot;      // Ship orientation in local planet frame

    glm::dvec3 F;      // Linear moment
    glm::dvec3 L;      // Angular moment
    glm::dvec3 Fadd;   // collecting linear forces
    glm::dvec3 Ladd;   // collecting torque components

};

class Vehicle : public VehicleBase
{
    friend class SuperVehicle;

public:
    Vehicle(cstr_t &name = "(self)");
    Vehicle(YAML::Node &config);
    virtual ~Vehicle();

    void clearModule();
    bool registerModule(cstr_t &name);
    bool loadModule(cstr_t &name);

    inline void clearTouchdownPoints()          { tpVertices.clear(); }

    void initLanded(Object *object, double lat, double lng, double dir);
    void initOrbiting(const glm::dvec3 &pos, const glm::dvec3 &vel, const glm::dvec3 &rot, const glm::dvec3 *vrot);
    void initDocked();

    void getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt) override;
    bool addSurfaceForces(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double tfrac, double dt) override;
    
    void setGenericDefaults();
    void setTouchdownPoints(const tdVertex_t *tdvtx, int ntd);

    void updateMass();
    virtual void update(bool force);

    void updateBodyForces();
    void updateAerodynamicForces();
    void updateThrustForces();

    void drawHUD(HUDPanel *hud, Sketchpad *pad);

    void createThruster(const glm::dvec3 &pos, const glm::dvec3 &dir, double maxth, tank_t *tank = nullptr);
    bool deleteThruster(thrust_t *th);

    // void setThrustLevel(thrust_t *th, double level);
    // void adjustThrustLevel(thrust_t *th, double dlevel);
    // void setThrustOverride(thrust_t *th, double level);
    // void adjustThrustOverride(thrust_t *th, double dlevel);

    void createThrusterGroup(thrustgrp_t *tg, thrustType_t type);

    void setThrustGroupLevel(thrustgrp_t *tg, double level);
    void adjustThrustGroupLevel(thrustgrp_t *tg, double dlevel);
    void setThrustGroupOverride(thrustgrp_t *tg, double level);
    void adjustThrustGroupOverride(thrustgrp_t *tg, double dlevel);

    void setThrustGroupLevel(thrustType_t type, double level);
    void adjustThrustGroupLevel(thrustType_t type, double dlevel);
    void setThrustGroupOverride(thrustType_t type, double level);
    void adjustThrustGroupOverride(thrustType_t type, double dlevel);

    double getThrustGroupLevel(thrustgrp_t *tg);
 
private:
    SuperVehicle *superVehicle = nullptr;

    ModuleHandle handle = nullptr;
    struct {
        VehicleModule *module = nullptr;
        int version = 0;
        ovcInit_t ovcInit = nullptr;
        ovcExit_t ovcExit = nullptr;
    } vif;

    double  lift;   // Lift force from wings
    double  drag;   // drag from atomspheric forces
    
    double  emass, fmass, pfmass; // empty mass, fuel mass, and previous fuel mass

    glm::dvec3 flin;        // linear moment (force)
    glm::dvec3 amom;        // angular moment (torque)
    glm::dvec3 cflin;       // Collecting linear force
    glm::dvec3 camom;       // Collecting torque force
    glm::dvec3 thrust;      // linear thrust force

    // Collision detection parameters (touchdown points)
    std::vector<tdVertex_t> tpVertices; // touchdown vertices (vessel frame)
    glm::dvec3 tpNormal;                   // upward normal of touchdown plane (vessel frame)
    glm::dvec3 tpCGravity;                 // center of gravity projection
    double  cogElev;

    std::vector<port_t *> ports;        // docking port list

    std::vector<MeshEntry *> meshList;

    std::vector<thrust_t *> thrustList;         // Individual thruster list
    std::vector<thrustgrp_t *> thgrpList;       // Main thruster group list
    std::vector<thrustgrp_t *> thgrpUserList;   // User thruster group list
};
