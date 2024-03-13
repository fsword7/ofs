// orbit.cpp - MFD Orbit Display package
//
// Author:  Tim Stark
// Date:    Nov 12, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "ephem/elements.h"
#include "engine/object.h"
#include "engine/vehicle.h"
#include "universe/celbody.h"
#include "control/panel.h"
#include "control/mfd/panel.h"
#include "control/mfd/orbit.h"

MFDOrbit::MFDOrbit(Panel *panel, const MFDSpec &spec, Vehicle *vehicle)
: MFDInstrument(panel, spec, vehicle)
{
    // refOrbit = vessel->getOrbitReference();
    shpOrbit = new OrbitalElements();
    tgtOrbit = new OrbitalElements();
    tgtVessel = nullptr;

    if (refOrbit != nullptr)
    {
        // shpOrbit->setup(vessel->getMass(), refOrbit->getMass(), vessel->getElements()->getMJDEpoch());
        // if (tgtVessel != nullptr)
        //     tgtOrbit->setup(tgtVessel->getMass(), refOrbit->getMass(), tgtVessel->getElements()->getMJDEpoch());
    }

    init(spec);
}

MFDOrbit::~MFDOrbit()
{
    if (shpOrbit != nullptr)
        delete shpOrbit;
    if (tgtOrbit != nullptr)
        delete tgtOrbit;
}

void MFDOrbit::init(const MFDSpec &spec)
{
    xCenter = spec.w / 2;
    yCenter = spec.h / 2;
    pixRadius  = spec.w * 4 / 9;
}

cchar_t *MFDOrbit::mfdGetButtonLabel(int idx)
{
    return nullptr;
}

void MFDOrbit::drawOrbitPath(Sketchpad *skpad, int which, const glm::dvec2 *path)
{
    // Draw orbital path line
    int norel = NOREL - (path[NOREL+2].x == -1) ? 1 : 0;
    skpad->drawPolygon(path, norel);

    // Draw radius vector line
    skpad->drawLine(xCenter, yCenter, path[NOREL].x, path[NOREL].y);

    // Draw apoapsis/periapsis markers
    glm::dvec2 pe = path[NOREL+1];
    glm::dvec2 ap = path[NOREL+2];
    skpad->drawEllipse(pe.x, pe.y, 3, 3);
    if (ap.x != -1)
        skpad->drawEllipse(ap.x, ap.y, 3, 3);

    // Draw ascending/descending node markers
    glm::dvec2 an = path[NOREL+3];
    glm::dvec2 dn = path[NOREL+4];
    skpad->drawRectangle(an.x, an.y, 3, 3);
    skpad->drawRectangle(dn.x, dn.y, 3, 3);
    skpad->drawLine(an.x, an.y, dn.x, dn.y);
}

void MFDOrbit::drawElements(Sketchpad *skpad, int x, int y, const OrbitalElements &orbit)
{

}

void MFDOrbit::draw(Sketchpad *skpad)
{

    double scale = 0;
    int rad = 0;
    bool instable;
    glm::dmat3 rot, irot;

    bool bEcliptic    = false;
    bool bValidShip   = (refOrbit != nullptr);
    bool bValidTarget = (refOrbit != nullptr && tgtVessel != nullptr);

    if (bValidShip)
    {
        glm::dvec3 pos = vehicle->getoPosition() - refOrbit->getoPosition();
        glm::dvec3 vel = vehicle->getoVelocity() - refOrbit->getoVelocity();
        shpOrbit->calculate(pos, vel, 0);
        scale = pixRadius / (shpOrbit->e < 1.0) ? shpOrbit->getApoapsis() :
            std::max(2.0 * shpOrbit->getPeriapsis(), shpOrbit->getRadius());

        // if (bValidTarget)
        // {
        //     glm::dvec3 pos = tgtVessel->getoPosition() - refOrbit->getoPosition();
        //     glm::dvec3 vel = tgtVessel->getoVelocity() - refOrbit->getoVelocity();
        //     tgtOrbit->calculate(pos, vel, 0);

        // }

        rad = (int)(refOrbit->getRadius() * scale + 0.5);
        instable = shpOrbit->getPeriapsis() < refOrbit->getRadius();
    }

    // Draw orbit paths
    {
        if (bValidShip == true)
            irot = getInverseRotMatrix(shpOrbit->cost, shpOrbit->sint, shpOrbit->cosi, shpOrbit->sini);
        else
            irot = glm::dmat3(1);

        // Draw planet ground (gray color)
        // skpad->setPen(drawColors[2][1].solidPen); // gray
        skpad->drawEllipse(xCenter, yCenter, rad, rad);

        if (bValidShip)
        {
            rot = getRotMatrix(shpOrbit->coso, shpOrbit->sino,
                shpOrbit->cost, shpOrbit->sint, shpOrbit->cosi, shpOrbit->sini);
            if (!bEcliptic)
                rot = irot * rot;
            updateOrbitPath(xCenter, yCenter, iWidth, iHeight, scale, *shpOrbit, rot, irot, shpPath);
            drawOrbitPath(skpad, 0, shpPath);
        }
    }

    // Display orbital elements
    {
        if (bValidShip)
        {
            // skpad->setTextColor(drawColors[0][0].solidPen); // Green
            drawElements(skpad, cw/2, (ch*2)/2, *shpOrbit);
        }
    }
}