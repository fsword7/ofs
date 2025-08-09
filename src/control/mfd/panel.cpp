// mfdpanel.cpp - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "engine/vehicle/vehicle.h"
#include "control/panel.h"
#include "control/mfd/panel.h"

DrawColor MFDInstrument::drawColors[3][2];

MFDInstrument::MFDInstrument(Panel *panel, const MFDSpec &spec, Vehicle *vehicle)
: panel(panel), flags(spec.flags), vehicle(vehicle)
{
    init(spec);
}

// MFDInstrument::~MFDInstrument()
// {
// }

void MFDInstrument::ginit(GraphicsClient *gc)
{
    // Set default colors
    drawColors[0][0].color = color_t(0.0, 1.0, 0.0); // Bright green
    drawColors[0][1].color = color_t(0.0, 0.5, 0.0); // Dark green
    drawColors[1][0].color = color_t(1.0, 1.0, 0.0); // Bright yellow
    drawColors[1][1].color = color_t(0.5, 0.5, 0.0); // Dark yellow
    drawColors[2][0].color = color_t(1.0, 1.0, 1.0); // White
    drawColors[2][1].color = color_t(0.5, 0.5, 0.5); // Gray

    // Create drawing solid/dashed pen colors
    for (int cidx = 0; cidx < 2; cidx++)
        for (int bidx = 0; bidx < 2; bidx++)
        {
            drawColors[cidx][bidx].solidPen  = gc->createPen(drawColors[cidx][bidx].color, 0, 0);
            drawColors[cidx][bidx].dashedPen = gc->createPen(drawColors[cidx][bidx].color, 0, 1);
        }
}

void MFDInstrument::gexit(GraphicsClient *gc)
{

}

MFDInstrument *MFDInstrument::create(Panel *panel, const MFDSpec &spec)
{
    return nullptr;
}

void MFDInstrument::init(const MFDSpec &spec)
{
    iWidth = spec.w;
    iHeight = spec.h;
    nbtl = spec.nbtl;
    nbtr = spec.nbtr;
    nbt  = spec.nbtl + spec.nbtr;
    bty0 = spec.bty0;
    btdy = spec.btdy;

    if (gc = ofsAppCore->getClient())
    {
        skpad = gc->getSketchpad();
        if (skpad != nullptr)
        {

        }
    }
}

Sketchpad *MFDInstrument::beginDraw()
{
    if (gc && skpad)
    {
        skpad->beginDraw();

        return skpad;
    }
    return nullptr;
}

void MFDInstrument::endDraw(Sketchpad *skpad)
{
    if (gc && skpad)
        skpad->endDraw();
}

bool MFDInstrument::update()
{

    if (Sketchpad *skp = beginDraw())
    {
        draw(skp);
        endDraw(skp);
    }

    return false;
}

glm::dmat3 MFDInstrument::getRotMatrix(double coso, double sino, double cosp, double sinp, double cosi, double sini)
{
    return glm::dmat3(cosp*coso - cosi*sino,      sinp*sini,      -cosp*sino-sinp*cosi*coso,
                      sini*sino,                  cosi,           sini*coso,
                      sinp*coso+cosp*cosi*sino,   -cosp*sini,     -sinp*sino+cosp*cosi*coso);
}

glm::dmat3 MFDInstrument::getInverseRotMatrix(double cosp, double sinp, double cosi, double sini)
{
    return glm::dmat3(cosp*cosp-sinp*sinp*cosi,   -sinp*sini,     cosp*sinp-sinp*cosp*cosi,
                      sinp*sini,                  cosi,           -cosp*sini,
                      sinp*cosp-cosp*sinp*cosi,   cosp*sini,      -sinp*sinp+cosp*cosp*cosi);
}

void MFDInstrument::mapScreen(int xCenter, int yCenter, double scale, const glm::dvec3 &vtx, glm::dvec2 *pt)
{
    pt->x = xCenter + (int)(vtx.x * scale);
    pt->y = yCenter - (int)(vtx.z * scale);
}

void MFDInstrument::updateEllipse(int xCenter, int yCenter, double scale, const OrbitalElements &orbit,
    const glm::dmat3 &rot, const glm::dmat3 &irot,  glm::dvec2 *pt)
{
    glm::dvec3 vtx[NOREL];
    double phi, sphi, cphi;
    double r, x, y;
    double fac = (pi/2)/NOREL;
    double e2 = orbit.e * orbit.e;
    glm::dvec3 an, dn;

    int idx1 = NOREL-1;
    int idx2 = NOREL4*2;
    int idx3 = idx2-1;

    for (int idx = 0; idx < NOREL4; idx++)
    {
        phi = (idx+0.5)*fac;
        sphi = sin(phi), cphi = cos(phi);
        r = orbit.getSemiMinorAxis() / sqrt(1.0 - e2*sphi*cphi);
        x = r*cphi, y = r*sphi;

        vtx[idx].x = vtx[idx1-idx].x = x - orbit.getLinearEccentricity();
        vtx[idx].z = vtx[idx3-idx].z = y;
        vtx[idx3-idx].x = vtx[idx2+idx].x = -x - orbit.getLinearEccentricity();
        vtx[idx2+idx].z = vtx[idx1-idx].z = -y;
    }

    // Set orbital path line
    for (int idx = 0; idx < NOREL; idx++)
        mapScreen(xCenter, yCenter, scale, rot * vtx[idx], pt+idx);

    // Set radius vector
    mapScreen(xCenter, yCenter, scale, irot * orbit.getoPosition(), pt+NOREL);

    // Set periapsis marker
    pt[NOREL+1].x = (pt[0].x + pt[NOREL-1].x)/2;
    pt[NOREL+1].y = (pt[0].y + pt[NOREL-1].y)/2;
    // Set apoapsis marker
    pt[NOREL+2].x = (pt[NOREL2-1].x + pt[NOREL2].x)/2;
    pt[NOREL+2].y = (pt[NOREL2-1].y + pt[NOREL2].y)/2;

    // Set ascending/descending node markers
    orbit.getAscendingNode(an);
    orbit.getDescendingNode(dn);
    mapScreen(xCenter, yCenter, scale, irot * an, pt+(NOREL+3));
    mapScreen(xCenter, yCenter, scale, irot * dn, pt+(NOREL+4));
}

void MFDInstrument::updateHyperboa(int xCenter, int yCenter, int iWidth, int iHeight, double scale, const OrbitalElements &orbit,
    const glm::dmat3 &rot, const glm::dmat3 &irot,  glm::dvec2 *pt)
{
    int idxh = NOREL2-1;
    glm::dvec3 vtx[NOREL-1];
    glm::dvec3 an, dn;
    double phi, cphi, sphi;
    double r, x, y, len;
    double p = orbit.getPeriapsisDistance() * (1.0 + orbit.e);
    double radMax = 1.5 * xCenter / scale;
    double phiMax = acos((p / radMax - 1.0) / orbit.e);
    double fac = phiMax / (double)(NOREL2-1);
    bool anok, dnok;

    vtx[idxh].x = orbit.getPeriapsisDistance();
    vtx[idxh].y = 0.0;

    for (int idx = 1; idx < NOREL2; idx++)
    {
        phi = idx*fac;
        sphi = sin(phi), cphi = cos(phi);
        r = p / (1.0 + orbit.e * cphi);
        x = r * cphi, y = r * sphi;
        vtx[idxh-idx].x = vtx[idxh+idx].x = x;
        vtx[idxh-idx].z = -(vtx[idxh+idx].z = y);
    }

    for (int idx = 0; idx < NOREL-1; idx++)
        mapScreen(xCenter, yCenter, scale, rot * vtx[idx], pt+idx);

    // Clipping hyperbolic orbit aganist the edge of MFD display
    for (int idx = 1; idx < NOREL2; idx++)
    {
        if (pt[idxh+idx].x < -10 * iWidth  || pt[idxh+idx].x >= 11 * iWidth ||
            pt[idxh+idx].y < -10 * iHeight || pt[idxh+idx].y >= 11 * iHeight)
            pt[idxh+idx].x = pt[idxh+idx-1].x, pt[idxh+idx].y = pt[idxh+idx-1].y;

        if (pt[idxh-idx].x < -10 * iWidth  || pt[idxh-idx].x >= 11 * iWidth ||
            pt[idxh-idx].y < -10 * iHeight || pt[idxh-idx].y >= 11 * iHeight)
            pt[idxh-idx].x = pt[idxh-idx+1].x, pt[idxh-idx].y = pt[idxh-idx+1].y;
    }

    // Set radius vector
    mapScreen(xCenter, yCenter, scale, irot * orbit.getoPosition(), pt+NOREL);

    // Set periapsis marker
    pt[NOREL+1].x = pt[idxh].x;
    pt[NOREL+1].y = pt[idxh].y;
    // Set apoapsis marker (invalid)
    pt[NOREL+2].x = -1;
    pt[NOREL+2].y = -1;

    // Set ascending/descending node markers
    if (anok = orbit.getAscendingNode(an))
    {
        if ((len = glm::length(an)) > radMax)
            an *= (radMax / len);
        mapScreen(xCenter, yCenter, scale, irot * an, pt+(NOREL+3));
    }
    if (dnok = orbit.getDescendingNode(dn))
    {
        if ((len = glm::length(dn)) > radMax)
            dn *= (radMax / len);
        mapScreen(xCenter, yCenter, scale, irot * dn, pt+(NOREL+4));
    }

    if (anok == false)
    {
        an = dn * -(radMax / len);
        mapScreen(xCenter, yCenter, scale, irot * an, pt+(NOREL+3));
    }
    else if (dnok == false)
    {
        dn = an * -(radMax / len);
        mapScreen(xCenter, yCenter, scale, irot * dn, pt+(NOREL+4));
    }
}

void MFDInstrument::updateOrbitPath(int xCenter, int yCenter, int iWidth, int iHeight, double scale, const OrbitalElements &orbit,
    const glm::dmat3 &rot, const glm::dmat3 &irot, glm::dvec2 *pt)
{
    if (orbit.e < 1.0)
        updateEllipse(xCenter, yCenter, scale, orbit, rot, irot, pt);
    else
        updateHyperboa(xCenter, yCenter, iWidth, iHeight, scale, orbit, rot, irot, pt);
}
