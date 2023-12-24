// orbit.h - MFD Orbit Display package
//
// Author:  Tim Stark
// Date:    Nov 12, 2023

#pragma once

class Vessel;

class MFDOrbit : public MFDInstrument
{
public:
    MFDOrbit(Panel *panel, const MFDSpec &spec, Vessel *vessel);
    virtual ~MFDOrbit();

    void init(const MFDSpec &spec);

    cchar_t *mfdGetButtonLabel(int idx) override;

    void drawOrbitPath(Sketchpad *skpad, int which, const glm::dvec2 *path);
    void drawElements(Sketchpad *skpad, int x, int y, const OrbitalElements &orbit);
    void draw(Sketchpad *skpad) override;

private:
    int xCenter, yCenter;
    float pixRadius;

    const CelestialBody *refOrbit = nullptr;
    const Object *tgtVessel = nullptr;

    OrbitalElements *shpOrbit = nullptr;
    OrbitalElements *tgtOrbit = nullptr;

    glm::dvec2 shpPath[NOREL+5];    // Orbital path data for spacecraft
    glm::dvec2 tgtPath[NOREL+5];    // Orbital path data for spacecraft (target)
};