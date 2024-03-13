// surface.h - MFD Surface Display package
//
// Author:  Tim Stark
// Date:    Nov 27, 2023

#pragma once

class Vessel;

class MFDSurface : public MFDInstrument
{
public:
    MFDSurface(Panel *panel, const MFDSpec &spec, Vehicle *vehicle);
    virtual ~MFDSurface();

    void init(const MFDSpec &spec);

    cchar_t *mfdGetButtonLabel(int idx) override;

    void draw(Sketchpad *skpad) override;

private:

    int spdx0;
    int hrzx, hrzx0, hrzx1, hrzc;
    int hrzy, hrzy0, hrzy1;;
};