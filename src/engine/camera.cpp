// camera.cpp - Camera package
//
// Author:  Tim Stark
// Date:    Sep 26, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/camera.h"

Camerax::Camerax(int w, int h)
{
    resize(w, h);
    fov = glm::radians(SCR_FOV);
}

void Camerax::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = float(width) / float(height);
}

void Camerax::setRelativePosition(double phi, double theta, double dist)
{
    double sph = sin(phi), cph = cos(phi);
    double sth = sin(theta), cth = cos(theta);

    rrot = {
        { cph, sph*sth,  -sph*cth },
        { 0.0, cth,      sth      },
        { sph, -cph*sth, cph*cth  }
    };
    rpos  = { rrot[0][2]*dist, rrot[1][2]*dist, rrot[2][2]*dist };
    rdist = dist;
}

void Camerax::orbit(double dPhi, double dTheta)
{
    setRelativePosition(ePhi+dPhi*0.5, eTheta+dTheta*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::orbitPhi(double dPhi)
{
    setRelativePosition(ePhi+dPhi*0.5, eTheta, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::orbitTheta(double dTheta)
{
    setRelativePosition(ePhi, eTheta+dTheta*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::update()
{

}

void Camerax::processMouse(int x, int y, int state)
{

}
