// surface.cpp - MFD Surface Display package
//
// Author:  Tim Stark
// Date:    Nov 27, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "ephem/elements.h"
#include "engine/object.h"
#include "engine/vessel.h"
#include "universe/celbody.h"
#include "control/panel.h"
#include "control/mfd/panel.h"
#include "control/mfd/surface.h"

MFDSurface::MFDSurface(Panel *panel, const MFDSpec &spec, Vessel *vessel)
: MFDInstrument(panel, spec, vessel)
{

}

MFDSurface::~MFDSurface()
{
}

void MFDSurface::init(const MFDSpec &spec)
{
}

cchar_t *MFDSurface::mfdGetButtonLabel(int idx)
{
    return nullptr;
}

void MFDSurface::draw(Sketchpad *skpad)
{
}