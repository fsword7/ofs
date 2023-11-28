// mfdpanel.cpp - Panel MFD (Multi-function display) package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "control/panel.h"
#include "control/mfd/panel.h"

MFDInstrument::MFDInstrument(Panel *panel, const Spec &spec)
: panel(panel), flags(spec.flags)
{
    init(spec);
}

// MFDInstrument::~MFDInstrument()
// {
// }

MFDInstrument *MFDInstrument::create(Panel *panel, const Spec &spec)
{
    return nullptr;
}

void MFDInstrument::init(const Spec &spec)
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
        // draw(skp);
        endDraw(skp);
    }

    return false;
}