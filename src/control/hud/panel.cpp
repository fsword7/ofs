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
#include "utils/json.h"

HUDPanel::HUDPanel(Panel *panel)
: panel(panel)
{

    gc = ofsAppCore->getClient();
    hudFont = gc->createFont("Arial", 30, false);

    panel->setHUDColor({0, 255, 0});
}

HUDPanel *HUDPanel::create(cjson &config, GraphicsClient *gc, Panel *panel)
{
    str_t hudName = myjson::getString<str_t>(config, "type");
    if (hudName.empty())
        return nullptr;
    HUDPanel *hud = nullptr;
    if (hudName == "surface")
        hud = new HUDSurfacePanel(panel);
    else if (hudName == "orbit")
        hud = new HUDOrbitPanel(panel);
    else {

    }

    if (hud != nullptr)
        hud->configure(config);

    return hud;
}

void HUDPanel::resize(int w, int h)
{
    width = w;
    height = h;

    cx = width / 2;
    cy = height / 2;

    hres05 = width / 2;
    vres05 = height / 2;
    lwidth = width * 0.12;
    lrange = height * 0.40;
    // hudofs = { 0, 0, cam->getScale() };
}

void HUDPanel::draw(Player &player, Sketchpad *pad)
{
    pad->beginDraw();
    display(player, pad);
    pad->endDraw();
}

void HUDPanel::drawCompassRibbon(Sketchpad *pad, double val)
{
    const int x0 = hres05;
    const int xmin = x0-(width/5);
    const int xmax = x0+(width/5);
    const int ymin = 20;
    const int ymax1 = height/80;
    const int ymax2 = ymax1*2;
    const int tsep2 = 16;

    double val2 = floor(val/2);
    val /= 2;
    int dx0 = int((val-val2) * tsep2 + 0.5);
    int d2 = int(val2 + 0.5);
    str_t sbuf;

    // ofsLogger->debug("Screen: width {} height {}\n", width, height);
    // ofsLogger->debug("Compass: x0 {} xmin {} xmax {}\n", x0, xmin, xmax);
    // ofsLogger->debug("Compass: ymin {} ymax1 {} ymax2 {}\n", ymin, ymax1, ymax2);
    // ofsLogger->debug("Heading: {} -> {}\n", val, dx0);

    pad->setFont(hudFont);
    pad->setTextColor({0, 255, 0});
    pad->setTextAlign(Sketchpad::CENTER);
    int fh = pad->getCharSize() & 0xFFFF;

    // draw arrow
    pad->moveTo(x0-5, ymin/2-3);
    pad->drawLineTo(x0, ymin-3);
    pad->drawLineTo(x0+5, ymin/2-3);

    for (int x = x0 - dx0, d = d2; x >= xmin; x -= tsep2, d--)
    {
        pad->moveTo(x, ymin);
        if (d % 5)
            pad->drawLineTo(x, ymin+ymax1);
        else
        {
            pad->drawLineTo(x, ymin+ymax2);
            if (d < 0)
                d += 180;
            sbuf = fmt::format("{:03d}", d*2);
            int w = pad->getTextWidth(sbuf.c_str(), 3) / 2;
            pad->text(x-w, ymax2+fh, sbuf.c_str(), 3);
        }
    }

    for (int x = x0 - dx0 + tsep2, d = d2+1; x <= xmax; x += tsep2, d++)
    {
        pad->moveTo(x, ymin);
        if (d % 5)
            pad->drawLineTo(x, ymin+ymax1);
        else
        {
            pad->drawLineTo(x, ymin+ymax2);
            if (d >= 180)
                d -= 180;
            sbuf = fmt::format("{:03d}", d*2);
            int w = pad->getTextWidth(sbuf.c_str(), 3) / 2;
            pad->text(x-w, ymax2+fh, sbuf.c_str(), 3);
        }
    }

    pad->setTextAlign(Sketchpad::LEFT);
}

void HUDPanel::drawLadderBar(Sketchpad *pad, double lcosb, double lsinb,
    double dcosb, double dsinb, double phi0, bool markSubzero)
{
    int x1, x2, y1, y2;
    int dx, dy;
    int dx1, dy1;
    int dx2, dy2;

    // bool markSubzero;
    bool isSubzero = (phi0 < 0);
    bool revert = false;

    if (phi0 > 9)
        phi0 = 18-phi0, revert = true;
    else if (phi0 < -9)
        phi0 = -18-phi0, revert = true;
    int lab = abs(phi0);

    x1 = cx + (int)( lcosb - dsinb + 0.5);
    x2 = cx + (int)(-lcosb - dsinb + 0.5);
    y1 = cy + (int)( lsinb + dcosb + 0.5);
    y2 = cy + (int)(-lsinb + dcosb + 0.5);
    dx = (x2 - x1);
    dy = (y2 - y1);
    dx1 = dx*3 / 8;
    dy1 = dy*3 / 8;

    pad->setFont(hudFont);
    pad->setTextColor({0, 255, 0});
    pad->setTextAlign(Sketchpad::CENTER);
    int fh = pad->getCharSize() & 0xFFFF;

    if (lab != 9) {
        if (lab != 0) {
            if (isSubzero) {
                dx2 = -dy / 20, dy2 = dx / 20;
                if (markSubzero)
                {
                    pad->drawLine(x1+dx1, y1+dy1, x1+dx/4, y1+dy/4);
                    pad->drawLine(x2-dx1, y2-dy1, x2-dx/4, y2-dy/4);
                    dx1 = dx / 8;
                    dy1 = dy / 8;
                }
            } else
                dx2 = dy / 20, dy2 = -dx / 20;
        }
        if (revert)
            dx2 = -dx2, dy2 = -dy2;
        pad->drawLine(x1+dx1, y1+dy1, x1, y1);
        if (lab != 0)
            pad->drawLineTo(x1+dx2, y1+dy2);
        pad->drawLine(x2-dx1, y2-dy1, x2, y2);
        if (lab != 0)
            pad->drawLineTo(x2+dx2, y2+dy2);
    } else {
        pad->drawLine(x1+dx1, y1+dy1, x1+dx/4, y1+dy/4);
        pad->drawLine(x2-dx1, y2-dy1, x2-dx/4, y2-dy/4);
        pad->drawLine(x1+dx/2+dy/8, y1+dy/2-dx/8, x1+dx/2+dy/4, y1+dy/2-dx/4);
        pad->drawLine(x1+dx/2-dy/8, y1+dy/2+dx/8, x1+dx/2-dy/4, y1+dy/2+dx/4);
    }

    str_t sbuf;
    if (markSubzero) {
        sbuf = fmt::format("{:.0f}", phi0*10);
        int fw = pad->getTextWidth(sbuf.c_str(), 3) / 2;
        pad->text(x1+dx/2-fw, y1+dy/2-fh/2, sbuf.c_str(), 3);
    } else {
        sbuf = fmt::format("{:02d}", lab*10);
        int fw = pad->getTextWidth(sbuf.c_str(), 2) / 2;
        pad->text(x1+dx/2-fw, y1+dy/2-fh/2, sbuf.c_str(), 2);
    }

    pad->setTextAlign(Sketchpad::LEFT);
}

void HUDPanel::drawDefault(Sketchpad *pad)
{

}

// ******** HUD Surface Panel ********

HUDSurfacePanel::HUDSurfacePanel(Panel *panel)
: HUDPanel(panel)
{
}

HUDSurfacePanel::~HUDSurfacePanel()
{
}

void HUDSurfacePanel::configure(cjson &config)
{

}

void HUDSurfacePanel::display(Player &player, Sketchpad *pad)
{
    vehicle = player.getVehicleTarget();
    csurface_t *sp = vehicle->getSurfaceParameters();
    if (sp == nullptr)
        return;
    cam = player.getCamera();

    pad->setPen(panel->getHUDPen());
    double heading = sp->heading - player.getCockpitTheta();

    drawCompassRibbon(pad, glm::degrees(heading));

    // draw horizon elevation ladder
    static double step = tan(glm::radians(10.0));
    double d = cam->getScale() * step;

    double phi0 = ofs::degrees(sp->pitch + player.getCockpitPhi())*0.1;
    double phi0f = floor(phi0);
    double bank0 = sp->bank + player.getCockpitTilt();
    double cosb = cos(bank0), sinb = sin(bank0);
    double lwcosb = lwidth * cosb, lwsinb = lwidth * sinb;

    if (d > lrange*0.1) {
        double d1, d0 = (phi0-phi0f) * d;
        int iphi, iphi0;
        iphi0 = iphi = (int)phi0;
    
        for (d1 = d0; d1 < lrange; d1 += d)
            drawLadderBar(pad, lwcosb, lwsinb, d1*cosb, d1*sinb, iphi--, true);
        iphi = iphi0+1;
        for (d1 = d0-d; d1 > -lrange; d1 -= d)
            drawLadderBar(pad, lwcosb, lwsinb, d1*cosb, d1*sinb, iphi++, true);
    }
}

// ******** HUD Orbit Panel ********

HUDOrbitPanel::HUDOrbitPanel(Panel *panel)
: HUDPanel(panel)
{
}

HUDOrbitPanel::~HUDOrbitPanel()
{
}

void HUDOrbitPanel::configure(cjson &config)
{

}

void HUDOrbitPanel::display(Player &player, Sketchpad *pad)
{

}