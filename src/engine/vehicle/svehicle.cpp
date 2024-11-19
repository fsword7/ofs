// svehicle.cpp - Super Vehicle package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "ephem/elements.h"
#include "engine/celestial.h"
#include "engine/rigidbody.h"
#include "engine/vehicle/vehicle.h"
#include "engine/vehicle/svehicle.h"

SuperVehicle::SuperVehicle(YAML::Node &config)
: RigidBody(config, objVehicle, cbVehicle)
{
    
}

bool SuperVehicle::addVehicle(Vehicle *vehicle1, int port1, Vehicle *vehicle2, int port2)
{
    int vidx;
    for (vidx = 0; vidx < vehicles.size(); vidx++)
        if (vehicles[vidx] == vehicle1)
            break;
    if (vidx == vehicles.size())
        return false;

    VehicleList entry;
    
    entry.vehicle = vehicle2;

    glm::dvec3 as = vehicle2->ports[port2]->dir;
    glm::dvec3 bs = vehicle2->ports[port2]->rot;
    glm::dvec3 cs = glm::cross(as, bs);

    glm::dvec3 at = vehicle1->ports[port1]->dir;
    glm::dvec3 bt = vehicle1->ports[port1]->rot;
    glm::dvec3 ct = glm::cross(at, bt);

    glm::dmat3 R;

    double den = cs.x * (as.y*bs.z - as.z*bs.y) +
                 cs.y * (as.z*bs.x - as.x*bs.z) +
                 cs.z * (as.x*bs.y - as.y*bs.x);

    R[0][0] = (ct.x * (as.y*bs.z - as.z*bs.y) +
               bt.x * (as.z*cs.y - as.y*cs.z) +
               at.x * (bs.y*cs.z - bs.z*cs.y)) / den;
    R[0][1] = (ct.x * (as.z*bs.x - as.x*bs.z) +
               bt.x * (as.x*cs.z - as.z*cs.x) +
               at.x * (bs.z*cs.x - bs.x*cs.z)) / den;
    R[0][2] = (ct.x * (as.x*bs.y - as.y*bs.x) +
               bt.x * (as.y*cs.x - as.x*cs.y) +
               at.x * (bs.x*cs.y - bs.y*cs.x)) / den;

    R[1][0] = (ct.y * (as.y*bs.z - as.z*bs.y) +
               bt.y * (as.z*cs.y - as.y*cs.z) +
               at.y * (bs.y*cs.z - bs.z*cs.y)) / den;
    R[1][1] = (ct.y * (as.z*bs.x - as.x*bs.z) +
               bt.y * (as.x*cs.z - as.z*cs.x) +
               at.y * (bs.z*cs.x - bs.x*cs.z)) / den;
    R[1][2] = (ct.y * (as.x*bs.y - as.y*bs.x) +
               bt.y * (as.y*cs.x - as.x*cs.y) +
               at.y * (bs.x*cs.y - bs.y*cs.x)) / den;

    R[2][0] = (ct.z * (as.y*bs.z - as.z*bs.y) +
               bt.z * (as.z*cs.y - as.y*cs.z) +
               at.z * (bs.y*cs.z - bs.z*cs.y)) / den;
    R[2][1] = (ct.z * (as.z*bs.x - as.x*bs.z) +
               bt.z * (as.x*cs.z - as.z*cs.x) +
               at.z * (bs.z*cs.x - bs.x*cs.z)) / den;
    R[2][2] = (ct.z * (as.x*bs.y - as.y*bs.x) +
               bt.z * (as.y*cs.x - as.x*cs.y) +
               at.z * (bs.x*cs.y - bs.y*cs.x)) / den;

    entry.rrot = R * vlist[vidx].rrot;
    entry.rq = entry.rrot;
    entry.rpos = vlist[vidx].rpos + (vlist[vidx].rrot * vehicle1->ports[port1]->port) -
        (entry.rrot * vehicle2->ports[port2]->port);

    // Vehicle2->setSuperVehicle(this);

    return true;
}

void SuperVehicle::attach(Vehicle *vehicle)
{
    // if (Vehicle->superVehicle == nullptr)
    //     addVehicle(Vehicle);
}

void SuperVehicle::detach(Vehicle *vehicle)
{
    for (std::vector<Vehicle *>::iterator p = vehicles.begin(); p != vehicles.end(); ++p)
        if (*p == vehicle) {
            vehicles.erase(p);
            break;
        }
}

void SuperVehicle::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt)
{

}

void SuperVehicle::updateMassCG()
{
    mass = 0.0;
    glm::dvec3 ncg = { 0, 0, 0};
    for(auto vdata : vlist)
    {
        double vmass = vdata.vehicle->getMass();
        mass += vmass;
        ncg  += vdata.rpos * vmass;
    }
    ncg /= mass;

    // Shift center gravity for superVehicle position
    // glm::dvec3 dp = s0->R * (cg - ncg);
    // s0->pos += dp;
    // rposAdd += dp;
    // cpos += dp;

    // Update center gravity
    cg = ncg;
}

void SuperVehicle::update(bool force)
{

    updateMassCG();

}
