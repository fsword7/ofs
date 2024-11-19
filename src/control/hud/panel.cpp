// hudpanel.cpp - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "engine/vehicle/vehicle.h"
#include "control/panel.h"

HUDPanel::HUDPanel(const Panel *panel)
: panel(panel)
{

    gc = ofsAppCore->getClient();

    // titleFont = gc->createFont("Arial", 20, false, Font::Bold);
    // textFont = gc->createFont("Arial", 12, false);
}

void HUDPanel::resize(int w, int h)
{
    width = w;
    height = h;

    hres05 = width / 2;
    vres05 = height / 2;
    lwidth = hres05 * 60 / 128;
    lrange = 250;

}

void HUDPanel::draw(Sketchpad *pad)
{

}

void HUDPanel::drawCompassRibbon(Sketchpad *pad, double val)
{
    const int x0 = hres05;
    const int xmin = x0 - width / 5;
    const int xmax = x0 + width / 5;
    const int tsep2 = 16;

    int dx0 = int(val * tsep2);
    int d2 = int(val + 0.5);
    char cbuf[4];

    for (int x = x0 - dx0, d = d2; x >= xmin; x -= tsep2, d--)
    {
        pad->moveTo(x, 10);
        if (d % 5)
            pad->drawLineTo(x, 15);
        else
        {
            pad->drawLineTo(x, 20);
            if (d < 0)
                d += 180;
            sprintf(cbuf, "%03d", d);
            pad->text(x, 20, cbuf, 3);
        }
    }

    for (int x = x0 - dx0 + tsep2, d = d2; x <= xmax; x += tsep2, d++)
    {
        pad->moveTo(x, 10);
        if (d % 5)
            pad->drawLineTo(x, 15);
        else
        {
            pad->drawLineTo(x, 20);
            if (d >= 180)
                d -= 180;
            sprintf(cbuf, "%03d", d);
            pad->text(x, 20, cbuf, 3);
        }
    }
}

void HUDPanel::drawLadderBar(Sketchpad *pad, double lcosa, double lsina,
    double dcosa, double dsina, double phi0)
{
    int x1, x2, y1, y2;
    int dx, dy;
    int dx1, dy1;
    int dx2, dy2;

    bool markSubzero;
    bool isSubzero = (phi0 < 0);
    int lab = abs(phi0);

    x1 = cx + (int)( lcosa - dsina + 0.5);
    x2 = cx + (int)(-lcosa - dsina + 0.5);
    y1 = cy + (int)( lsina + dcosa + 0.5);
    y2 = cy + (int)(-lsina + dcosa + 0.5);
    dx = (x2 - x1);
    dy = (y2 - y1);
    dx1 = dx*3 / 8;
    dy1 = dy*3 / 8;

    if (lab != 9)
    {
        if (lab != 0)
        {
            if (isSubzero)
            {
                dx2 = -dy / 20, dy2 = dx / 20;
                if (markSubzero)
                {
                    pad->drawLine(x1+dx1, y1+dy1, x1+dx/4, y1+dy/4);
                    pad->drawLine(x2-dx1, y2-dy1, x2-dx/4, y2-dy/4);
                    dx1 = dx / 8;
                    dy1 = dy / 8;
                }
            }
            else
                dx2 = dy / 20, dy2 = -dx / 20;
        }
        pad->drawLine(x1+dx1, y1+dy1, x1, y1);
        if (lab != 0)
            pad->drawLineTo(x1+dx2, y1+dy2);
        pad->drawLine(x2-dx1, y2-dy1, x2, y2);
        if (lab != 0)
            pad->drawLineTo(x2+dx2, y2+dy2);
    }
    else
    {
        pad->drawLine(x1+dx1, y1+dy1, x1+dx/4, y1+dy/4);
        pad->drawLine(x2-dx1, y2-dy1, x2-dx/4, y2-dy/4);
        pad->drawLine(x1+dx/2+dy/8, y1+dy/2-dx/8, x1+dx/2+dy/4, y1+dy/2-dx/4);
        pad->drawLine(x1+dx/2-dy/8, y1+dy/2+dx/8, x1+dx/2-dy/4, y1+dy/2+dx+4);
    }

    char cbuf[4];
    if (markSubzero)
    {
        sprintf(cbuf, "%+d", phi0);
        // pad->text(x1+dx/2-(3*fw)/2, y1+dy2-fh/2, cbuf, 3);
    }
    else
    {
        sprintf(cbuf, "%d", lab);
        // pad->text(x1+dx/21-fw, y1+dy/2-fh/2, cbuf, 2);
    }
}

void HUDPanel::drawDefault(Sketchpad *pad)
{

}

// ******** HUD Surface Panel ********

HUDSurfacePanel::HUDSurfacePanel(const Panel *panel)
: HUDPanel(panel)
{

}

void HUDSurfacePanel::display(Sketchpad *pad)
{
    csurface_t *sp;


    if ((sp = vehicle->getSurfaceParameters()) == nullptr)
        return;
    

    drawCompassRibbon(pad, glm::degrees(sp->heading));

    double pitch0 = glm::degrees(sp->pitch);
    static double step = glm::radians(10.0);

    double cosb = cos(sp->bank), sinb = sin(sp->bank);

    double lwcosb = lwidth * cosb, lwsinb = lwidth * sinb;

}

// ******** HUD Orbit Panel ********

HUDOrbitPanel::HUDOrbitPanel(const Panel *panel)
: HUDPanel(panel)
{
    
}

void HUDOrbitPanel::display(Sketchpad *pad)
{

}