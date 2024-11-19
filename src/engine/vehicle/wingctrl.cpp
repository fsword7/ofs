// wingctrl.cpp - Vehicle package - aerodynamics
//
// Author:  Tim Stark
// Date:    Oct 21, 2024

#include "main/core.h"
#include "engine/vehicle/vehicle.h"

airfoil_t *Vehicle::createAirfoil(airfoilType_t align, const glm::dvec3 &ref, affuncx_t cf, void *ctx, double c, double S, double A)
{
    assert(cf != nullptr);

    airfoil_t *wing = new airfoil_t();

    wing->align = align;
    wing->ref = ref;
    wing->c = c;
    wing->A = A;
    wing->S = S;

    wing->cfx = cf;
    wing->ctx = ctx;

    airfoilList.push_back(wing);

    return wing;
}

afctrl_t *Vehicle::createControlSurface(ControlType_t type, glm::dvec3 &ref,
    double area, double dcl, double axis, double delay, unsigned int anim)
{
    afctrl_t *ctrl = new afctrl_t();

    ctrl->type = type;
    ctrl->ref = ref;
    ctrl->area = area;
    ctrl->dcl = dcl;
    ctrl->anim = anim;
    ctrl->axis = axis;

    afctrlLevels[type].delay = delay;

    // if (axis == axisAuto)
    // {
    //     switch(type)
    //     {
    //     case aircAileron:
    //         ctrl->axis = (ref.x > 0 ? axisXpos : axisXneg);
    //         break;
    //     case aircFlaps:
    //         ctrl->axis = axisXpos;
    //         break;

    //     case aircElevator:
    //     case aircElevatorTrim:
    //         ctrl->axis = axisXpos;
    //         break;

    //     case aircRudder:
    //     case aircRudderTrim:
    //         ctrl->axis = axisYpos;
    //         break;
    //     }
    // }

    afctrlList.push_back(ctrl);

    return ctrl;
}

void Vehicle::setControlSurfaceLevel(ControlType_t ctrl, double level, bool transient, bool direct)
{
    if (transient == true)
        afctrlLevels[ctrl].tgtt = level;
    else
        afctrlLevels[ctrl].tgtp = level;
}

void Vehicle::updateControlSurfaceLevels()
{
    // Clear all change flags
    bool bChange = false;
    bool bChangeControl[AIRCTRL_NLEVEL];
    for (int idx = 0; idx < AIRCTRL_NLEVEL; idx++)
        bChangeControl[idx] = false;

    for (int idx = 0; idx < AIRCTRL_NLEVEL; idx++) {
        // level = std::max(-1.0, std::min(1.0, afctrlLevels[idx].tgtp + afctrlLevels[idx].tgtt));
        double tgtlvl = std::clamp(afctrlLevels[idx].tgtp + afctrlLevels[idx].tgtt, -1.0, 1.0);
        if (tgtlvl != afctrlLevels[idx].curr) {
            double level = tgtlvl;
            if (afctrlLevels[idx].delay > 0) {
                // Adjust level gradually
                double dlevel = level - afctrlLevels[idx].curr;
                double dmax = ofsDate->getSimDeltaTime1() / afctrlLevels[idx].delay;
                if (dlevel > dmax)
                    level = afctrlLevels[idx].curr + dmax;
                else if (dlevel < dmax)
                    level = afctrlLevels[idx].curr - dmax;
            }
            afctrlLevels[idx].curr = level;
            bChange = bChangeControl[idx] = true;
        }
    }

    // Applying animations
}

void Vehicle::updateAerodynamicForces()
{
    if (airfoilList.empty())
        return;
    surface_t &sp = surfParam;
    if (sp.avlocal.z == 0.0)
        return;

    double aoa = atan2(-sp.avlocal.y, sp.avlocal.z);
    double beta = atan2(-sp.avlocal.x, sp.avlocal.z);

    double Re0 = (sp.atmDensity * sp.airSpeed) / mulat;

    glm::dvec3 ddir = glm::normalize(-sp.avlocal);                          // drag direction
    glm::dvec3 ldir = glm::normalize(glm::dvec3(0, sp.avlocal.z, -sp.avlocal.y ));   // lift direction
    glm::dvec3 sdir = glm::normalize(glm::dvec3(sp.avlocal.z, 0, -sp.avlocal.x ));   // sidelift direction
    double l, d, S;                                                         // lift, drag, 
    double cl, cm, cd;                                                      // lift, moment, drag coeffs

    for (auto &af : airfoilList)
    {
        switch (af->align)
        {
        case afHorizontal:
            if (af->cfx != nullptr)
                af->cfx(this, beta, sp.atmMach, Re0 * af->c, af->ctx, cl, cm, cd);
            S = (af->S != 0.0) ? af->S : fabs(ddir.z)*cs.z + fabs(ddir.x)*cs.z;
            l = cl * sp.atmPressure * S;
            d = cd * sp.atmPressure * S;
            addForce(sdir*l + ddir*d, af->ref);
            if (cm != 0)
                camom.y += cm + sp.atmPressure * af->S * af->c;
            drag += d;
            break;

        case afVertical:
            if (af->cfx != nullptr)
                af->cfx(this, aoa, sp.atmMach, Re0 * af->c, af->ctx, cl, cm, cd);
            S = (af->S != 0.0) ? af->S : fabs(ddir.z)*cs.z + fabs(ddir.y)*cs.y;
            l = cl * sp.atmPressure * S;
            d = cd * sp.atmPressure * S;
            addForce(ldir*l + ddir*d, af->ref);
            if (cm != 0)
                camom.x += cm + sp.atmPressure * af->S * af->c;
            lift += l, drag += d;  
            break;
        }
    }

    for (auto &ctrl : afctrlList)
    {
        double clift;
        double cdrag;
        switch (ctrl->axis)
        {
        case 0: // horizontal control (+X), elevator/aileron
            addForce(ldir*clift + ddir*cdrag, ctrl->ref);
            break;
        case 1: // horizontal control (-X), elevator/aileron
            addForce(ldir*-clift + ddir*cdrag, ctrl->ref);
            break;
        case 2: // vertical control (+Y), rubber
            addForce(sdir*clift + ddir*cdrag, ctrl->ref);
            break;
        case 3: // vetical control (-Y), rubber
            addForce(sdir*-clift + ddir*cdrag, ctrl->ref);
            break;
        }
    }
}
