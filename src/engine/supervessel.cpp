// supervessel.cpp - super vessel package
//
// Author:  Tim Stark
// Date:    Apr 25, 2022

#include "main/core.h"
#include "ephem/elements.h"
#include "engine/rigidbody.h"
#include "engine/vessel.h"
#include "engine/supervessel.h"

SuperVessel::SuperVessel()
: RigidBody("", objVessel)
{
    
}

bool SuperVessel::addVessel(Vessel *vessel1, int port1, Vessel *vessel2, int port2)
{
    int vidx;
    for (vidx = 0; vidx < vessels.size(); vidx++)
        if (vessels[vidx] == vessel1)
            break;
    if (vidx == vessels.size())
        return false;

    VesselList entry;
    
    entry.vessel = vessel2;

    glm::dvec3 as = vessel2->ports[port2]->dir;
    glm::dvec3 bs = vessel2->ports[port2]->rot;
    glm::dvec3 cs = glm::cross(as, bs);

    glm::dvec3 at = vessel1->ports[port1]->dir;
    glm::dvec3 bt = vessel1->ports[port1]->rot;
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
    entry.rpos = vlist[vidx].rpos + (vlist[vidx].rrot * vessel1->ports[port1]->port) -
        (entry.rrot * vessel2->ports[port2]->port);

    // vessel2->setSupervessel(this);

    return true;
}

void SuperVessel::attach(Vessel *vessel)
{
    // if (vessel->superVessel == nullptr)
    //     addVessel(vessel);
}

void SuperVessel::detach(Vessel *vessel)
{
    for (std::vector<Vessel *>::iterator p = vessels.begin(); p != vessels.end(); ++p)
        if (*p == vessel) {
            vessels.erase(p);
            break;
        }
}

void SuperVessel::getIntermediateMoments(glm::dvec3 &acc, glm::dvec3 &am, const StateVectors &state, double dt)
{

}

void SuperVessel::updateMassCG()
{
    mass = 0.0;
    glm::dvec3 ncg = { 0, 0, 0};
    for(auto vdata : vlist)
    {
        double vmass = vdata.vessel->getMass();
        mass += vmass;
        ncg  += vdata.rpos * vmass;
    }
    ncg /= mass;

    // Shift center gravity for supervessel position
    // glm::dvec3 dp = s0->R * (cg - ncg);
    // s0->pos += dp;
    // rposAdd += dp;
    // cpos += dp;

    // Update center gravity
    cg = ncg;
}

void SuperVessel::update(bool force)
{

    updateMassCG();

}
