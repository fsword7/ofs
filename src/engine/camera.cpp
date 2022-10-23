// camera.cpp - Camera package
//
// Author:  Tim Stark
// Date:    Sep 26, 2022

#include "main/core.h"
#include "main/app.h"
#include "engine/object.h"
#include "engine/camera.h"

Camerax::Camerax(int w, int h)
{
    resize(w, h);
    fov = glm::radians(SCR_FOV);
    zNear = 1.0;
    zFar  = 1e10;
}

void Camerax::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = float(width) / float(height);
}

void Camerax::look(const glm::dvec3 &opos)
{
    glm::dvec3 up = { 0, 1, 0 };
    
    urot = glm::lookAt(upos, opos, up);
}

void Camerax::setRelativePosition(double phi, double theta, double dist)
{
    ePhi = phi;
    eTheta = theta;

    double sph = sin(phi), cph = cos(phi);
    double sth = sin(theta), cth = cos(theta);

    rrot = {
        { cph, sph*sth,  -sph*cth },
        { 0.0, cth,      sth      },
        { sph, -cph*sth, cph*cth  }
    };
    rpos  = { rrot[0][2]*dist, rrot[1][2]*dist, rrot[2][2]*dist };
    rdist = dist;
}

void Camerax::orbit(double dPhi, double dTheta)
{
    setRelativePosition(ePhi+dPhi*0.5, eTheta+dTheta*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::orbitPhi(double dPhi)
{
    setRelativePosition(ePhi+dPhi*0.5, eTheta, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::orbitTheta(double dTheta)
{
    setRelativePosition(ePhi, eTheta+dTheta*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camerax::rotate(double dPhi, double dTheta)
{
    // Update current phi (X) and theta (Y)
    ePhi   += dPhi;
    eTheta += dTheta;

    double sph = sin(ePhi), cph = cos(ePhi);
    double sth = sin(eTheta), cth = cos(eTheta);

    rrot = {
        { cph, sph*sth,  -sph*cth },
        { 0.0, cth,      sth      },
        { sph, -cph*sth, cph*cth  }
    };
}

void Camerax::rotatePhi(double dPhi)
{

}

void Camerax::rotateTheta(double dTheta)
{

}

void Camerax::attach(Object *object, extCameraMode nMode)
{
    // Assign object (planet, vessel, etc) to target
    // as relative coordinates
    targetObject = object;

    if (modeExternal == true)
    {
        setRelativePosition(ePhi, eTheta, rdist);

    }
}

void Camerax::update()
{


    switch (mode)
    {
    case modeGlobalFrame:
        lpos = rpos;
        upos = lpos; //targetObject->getGlobalPosition() + lpos;
        urot = rrot;
        break;

    }
}

void Camerax::processKeyboard()
{

}

// ******** Mouse Controls ********

void Camerax::mouseMove(float mx, float my, int state)
{

}

void Camerax::mousePressButtonDown(float mx, float my, int state)
{
    if (state & CoreApp::mouseLeftButton)
    {

    }
}

void Camerax::mousePressButtonUp(float mx, float my, int state)
{
    // View *view = nullptr;
    // float vx = 0.0f, vy = 0.0f;

    // if (state & mouseLeftButton)
    // {
    //     view = pickView(mx, my);
    //     if (view != nullptr)
    //         view->map(mx / float(width), my / float(height), vx, vy);

    //     vec3d_t ray = player->getPickRay(vx, vy);

    //     Object *picked = engine->pickObject(ray);
    // }
}

void Camerax::mouseDialWheel(float motion, int state)
{

}
