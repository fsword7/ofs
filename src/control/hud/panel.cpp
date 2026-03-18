// hudpanel.cpp - HUD overlay screen package
//
// Author:  Tim Stark
// Date:    Nov 11, 2023

#include "main/core.h"
#include "main/app.h"
#include "api/graphics.h"
#include "api/draw.h"
#include "engine/celestial.h"
#include "engine/vehicle/vehicle.h"
#include "control/panel.h"
#include "utils/json.h"

HUDPanel::HUDPanel(Panel *panel)
: panel(panel)
{

    gc = ofsAppCore->getClient();
    hudFont = gc->createFont("Arial", 30, false);

    panel->setHUDColor({0, 1, 0});
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

    markerSize = std::max(20, height/28);

    // hudofs = { 0, 0, cam->getScale() };
}

void HUDPanel::draw(Player &player, Sketchpad *pad)
{
    pad->beginDraw();
    display(player, pad);
    pad->endDraw();
}

void HUDPanel::drawDefault(Sketchpad *pad)
{

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
            sbuf = std::format("{:03d}", d*2);
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
            sbuf = std::format("{:03d}", d*2);
            int w = pad->getTextWidth(sbuf.c_str(), 3) / 2;
            pad->text(x-w, ymax2+fh, sbuf.c_str(), 3);
        }
    }

    pad->setTextAlign(Sketchpad::LEFT);
}

void HUDPanel::drawTiltedRibbon(Sketchpad *pad, double phi, double alpha)
{
    double step = tan(ofs::radians(2.0));
    double d = cam->getScale() * step;
    if (d < lrange*0.01)
        return;
    double cosa = cos(alpha), sina = sin(alpha);
    double s = lwidth * 0.094;
    int dsx = int(s*cosa);
    int dsy = int(s*sina);
    int dtx = int(0.5*s*cosa);
    int dty = int(0.5*s*sina);
    int x0, y0, iphin;

    double phi0 = floor(ofs::degrees(phi)*0.5);
    double d0 = (phi0 - ofs::degrees(phi)*0.5) * d, d1;

    pad->setFont(hudFont);
    pad->setTextColor({0, 255, 0});
    pad->setTextAlign(Sketchpad::CENTER);
    int fh = pad->getCharSize() & 0xFFFF;

    int iphi0, iphi;
    iphi0 = (int)phi0;

    for (d1 = d0, iphi = iphi0; d1 > -lrange*0.7; d1 -= d, iphi--)
    {
        iphin = iphi*2;
        if (iphin >= 360)
            iphin -= 360;
        else if (iphin < 0)
            iphin += 360;
        x0 = cx + int(d1*cosa);
        y0 = cy + int(d1*sina);
        if (iphin % 10) {
            pad->drawLine(x0-dty, y0+dtx, x0+dty, y0-dtx);
        } else {
            pad->drawLine(x0-dsy, y0+dsx, x0+dsy, y0-dsx);
            str_t sbuf = std::format("{:03d}", iphin);
            int fw = pad->getTextWidth(sbuf.c_str(), 3) / 2;
            pad->text(x0-fw, y0-dsx*2-fh/2, sbuf.c_str(), 3);
        }
    }
    
    for (d1 = d0+d, iphi = iphi0+1; d1 < lrange*0.7; d1 += d, iphi++)
    {
        iphin = iphi*2;
        if (iphin >= 360)
            iphin -= 360;
        else if (iphin < 0)
            iphin += 360;
        x0 = cx + int(d1*cosa);
        y0 = cy + int(d1*sina);
        if (iphin % 10) {
            pad->drawLine(x0-dty, y0+dtx, x0+dty, y0-dtx);
        } else {
            pad->drawLine(x0-dsy, y0+dsx, x0+dsy, y0-dsx);
            str_t sbuf = std::format("{:03d}", iphin);
            int fw = pad->getTextWidth(sbuf.c_str(), 3) / 2;
            pad->text(x0-fw, y0-dsx*2-fh/2, sbuf.c_str(), 3);
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
        sbuf = std::format("{:.0f}", phi0*10);
        int fw = pad->getTextWidth(sbuf.c_str(), 3) / 2;
        pad->text(x1+dx/2-fw, y1+dy/2-fh/2, sbuf.c_str(), 3);
    } else {
        sbuf = std::format("{:02d}", lab*10);
        int fw = pad->getTextWidth(sbuf.c_str(), 2) / 2;
        pad->text(x1+dx/2-fw, y1+dy/2-fh/2, sbuf.c_str(), 2);
    }

    pad->setTextAlign(Sketchpad::LEFT);
}

bool HUDPanel::checkVisualArea(const glm::dvec3 &gdir, glm::dvec3 &vscr)
{
    vscr = cam->getProjViewMatrix() * glm::dvec4(gdir, 1.0);
    // ofsLogger->debug("Visual: {},{},{}\n", vscr.x, vscr.y, vscr.z);
    return (vscr.x >= -1.0 && vscr.x <= 1.0) &&
           (vscr.y >= -1.0 && vscr.y <= 1.0) &&
           (vscr.z >= 0.0);
}

bool HUDPanel::getXY(Celestial *obj, const glm::dvec3 &dir, int &x, int &y)
{
    bool vis = false;

    // glm::dvec3 ldir = glm::transpose(obj->getgRotation()) * dir;
    // if (ldir.z > 0) {
    //     double fac = hudofs.z / ldir.z;
    //     if (fabs(fac) > 10000.0)
    //         return false;
    //     x = cx + int(fac * ldir.x);
    //     y = cy + int(fac * ldir.y);
    // } else
    //     vis = false;

    glm::dvec3 vscr;
    if (vis = checkVisualArea(dir, vscr)) {
        x = width/2 * (1.0+vscr.x);
        y = height/2 * (1.0+vscr.y);
        // ofsLogger->debug("Visual: ({},{}) -> ({},{})\n",
        //     vscr.x, vscr.y, x, y);
    }
    return vis;
}

bool HUDPanel::getXY(Celestial *obj, const glm::dvec3 &dir, double &x, double &y)
{
    bool vis = false;

    // glm::dvec3 ldir = glm::transpose(obj->getgRotation()) * dir;
    // if (ldir.z > 0) {
    //     double fac = hudofs.z / ldir.z;
    //     if (fabs(fac) > 10000.0)
    //         return false;
    //     x = cx + int(fac * ldir.x);
    //     y = cy + int(fac * ldir.y);
    // } else
    //     vis = false;

    glm::dvec3 vscr;
    if (vis = checkVisualArea(dir, vscr)) {
        x = width/2 * (1.0+vscr.x);
        y = height/2 * (1.0+vscr.y);
        // ofsLogger->debug("Visual: ({}.{}) -> ({}, {})",
        //     vscr.x, vscr.y, x, y);
    }
    return vis;
}

bool HUDPanel::drawCenterMarker(Sketchpad *pad)
{
    int d = width/48;

    pad->drawLine(cx-2*d, cy, cx-d, cy);
    pad->drawLine(cx+2*d, cy, cx+d, cy);

    pad->moveTo(cx-d, cy+d);
    pad->drawLineTo(cx, cy);
    pad->drawLineTo(cx+d+1, cy+d+1);

    return true;
}

bool HUDPanel::drawMarker(Sketchpad *pad, Celestial *obj, const glm::dvec3 &dir, int style)
{
    int x, y;
    if (getXY(obj, dir, x, y)) {
        if (style & 1) {
            // Draw a square marker
            pad->drawRectangle(x-markerSize, y-markerSize, x+markerSize+1, y+markerSize+1);
        }
        if (style & 2) {
            // Draw a circle marker
            pad->drawEllipse(x-markerSize, y-markerSize, x+markerSize+1, y+markerSize+1);
        }
        if (style & 4) {
            // Draw a cross marker
            pad->drawLine(x-markerSize, y, x+markerSize+1, y);
            pad->drawLine(x, y-markerSize, x, y+markerSize+1);
        }
        return true;
    }
    return false;
}

bool HUDPanel::drawOffscreenDirMarker(Sketchpad *pad, const glm::dmat3 &grot, const glm::dvec3 &dir, str_t text)
{
    static glm::dvec2 pt[4];
    glm::dvec3 ldir = glm::transpose(grot) * dir;

    double len = std::hypot(ldir.y, ldir.x);
    double scale = markerSize * 0.6;
    double dx = ldir.x / len * scale;
    double dy = ldir.y / len * scale;

    pt[0] = { cx + 10.0*dx, cy - 10.0*dy };
    pt[1] = { cx + 7.0*dx - dy, cy - 7.0*dy + dx };
    pt[2] = { cx + 7.0*dx + dy, cy - 7.0*dy - dx };
    pt[3] = { cx + 8.5*dx, cy - 8.5*dy };

    pad->drawPolygon(pt, 4);
    if (!text.empty()) {
        int fw = pad->getTextWidth(text.c_str(), text.size()) / 2;
        int fh = pad->getCharSize() & 0xFFFF;
        pad->text(pt[3].x-fw, pt[3].y-markerSize-fh-2, text.c_str(), text.size());
    }

    return false;
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
    // double heading = sp->heading - player.getCockpitTheta();
    double heading = sp->heading;

    drawCompassRibbon(pad, glm::degrees(heading));

    // drawMarker(pad, nullptr, {}, 6);

    // draw horizon elevation ladder
    static double step = tan(glm::radians(10.0));
    double d = cam->getScale() * step;

    // double phi0 = ofs::degrees(sp->pitch + player.getCockpitPhi())*0.1;
    double phi0 = ofs::degrees(sp->pitch)*0.1;
    double phi0f = floor(phi0);
    // double bank0 = sp->bank + player.getCockpitTilt();
    double bank0 = sp->bank;
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

    // ofsLogger->debug("Heading: {}, Pitch: {}, Bank: {}\n",
    //     ofs::degrees(sp->heading), ofs::degrees(sp->pitch), ofs::degrees(sp->bank));
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
    vehicle = player.getVehicleTarget();
    Celestial *center = vehicle->getOrbitalReference();
    if (center == nullptr)
        return;
    cam = player.getCamera();

    pad->setPen(panel->getHUDPen());

    glm::dvec3 rvel = vehicle->getgVelocity() - center->getgVelocity();
    glm::dvec3 dvel = glm::normalize(rvel);
    glm::dvec3 lvel = glm::normalize(vehicle->getgRotation() * dvel);
    if (!drawMarker(pad, center, dvel, 6) &&
        !drawMarker(pad, center, -dvel, 4)) {
        // glm::dmat3 grot = cam->getGlobalRotation();
        // drawOffscreenDirMarker(pad, grot, dvel, "PG");
    }

    glm::dvec3 rpos = glm::transpose(vehicle->getgRotation()) * (vehicle->getgPosition() - center->getgPosition());
    glm::dvec3 dpos = glm::normalize(rpos);
    double a = dpos.z*lvel.y - dpos.y*lvel.z;
    double b = dpos.x*lvel.z - dpos.z*lvel.x;
    double c = dpos.y*lvel.x - dpos.x*lvel.y;


    double h = 1.0 / sqrt(a*a + b*b + c*c);
    double phi = asin(c * h);
    double alpha = atan2(a, b);
    double sina = sin(alpha), cosa = cos(alpha);
    double lwcosa = lwidth * cosa, lwsina = lwidth * sina;

    double phi0 = floor(ofs::degrees(phi)*0.1);
    static double step = tan(ofs::radians(10.0));
    double d0, d1, d = cam->getScale() * step;
    int iphi, iphi0;

    if (d > lrange*0.1) {
        d0 = (1.0*ofs::degrees(phi) - phi0) * d;
        iphi0 = iphi = (int)phi0;
        for (d1 = d0; d1 < lrange; d1 += d)
            drawLadderBar(pad, lwcosa, lwsina, d1*cosa, d1*sina, iphi--, true);
        iphi = iphi0+1;
        for (d1 = d0-d; d1 > -lrange; d1 -= d)
            drawLadderBar(pad, lwcosa, lwsina, d1*cosa, d1*sina, iphi++, true);          
    }

    glm::dvec3 z0 = { -c*a, -c*b, a*a + b*b };
    if (z0.x || z0.y || z0.z) {
        z0 = glm::normalize(z0);
        double cosa = glm::dot(z0, lvel);
        double phi = acos(cosa);
        glm::dvec3 vv = { lvel.y*c - lvel.z*b, lvel.z*a - lvel.x*c, lvel.x*b - lvel.y*a };
        if (glm::dot(vv, z0) > 0)
            phi = pi2-phi;
        drawTiltedRibbon(pad, phi, alpha);
    }

    // drawCenterMarker(pad);

    // ofsLogger->debug("Orbit Direction:   ({}, {}, {})\n", dvel.x, dvel.y, dvel.z);
    // ofsLogger->debug("Vehicle Direction: ({}, {}, {})\n", lvel.x, lvel.y, lvel.z);
}