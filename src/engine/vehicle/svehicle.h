// svehicle.h - Super Vehicle package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#pragma once

#include "engine/rigidbody.h"

struct VehicleList
{
    Vehicle *vehicle;   // Attached/docked vessel
    glm::dvec3 rpos;    // relative position in supervessel coords
    glm::dmat3 rrot;    // relative orientation (rotation matrix)
    glm::dquat rq;      // relative orientation (quaternion)
};

class SuperVehicle : public RigidBody
{
public:
    SuperVehicle();
    SuperVehicle(json &config);
    virtual ~SuperVehicle() = default;

    bool addVehicle(Vehicle *vehicle1, int port1, Vehicle *vehicle2, int port2);
    void attach(Vehicle *vehicle);
    void detach(Vehicle *vehicle);
    
    void getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt);

    void updateMassCG();

    void update(bool force);

private:
    std::vector<Vehicle *> vehicles;
    std::vector<VehicleList> vlist;
};