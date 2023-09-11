// camera.cpp - Camera package
//
// Author:  Tim Stark
// Date:    Sep 26, 2022

#include "main/core.h"
#include "main/app.h"
#include "engine/object.h"
#include "engine/camera.h"

Camera::Camera(int w, int h)
{
    resize(w, h);
    fov = glm::radians(SCR_FOV);
    zNear = 1.0;
    zFar  = 1e24; // known universe size (1e11 lightyears)

    reset();
}

void Camera::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = float(width) / float(height);
}

void Camera::reset()
{
    erot  = { 0, 0, 0 };
    clrot = { 0, 0, 0 };

    rpos  = { 0, 0, 0 };
    rrot  = glm::dmat3(1);
    rdist = 0;

    updateProjectionMatrix();
}

void Camera::updateProjectionMatrix()
{
    proj = ofs::perspective(fov, aspect, zNear, zFar);
}

void Camera::setPosition(const glm::dvec3 &pos)
{
    rpos  = pos;
    rdist = glm::length(rpos);
}

void Camera::setRotation(const glm::dmat3 &rot)
{
    rrot = rot;
}

void Camera::look(const glm::dvec3 &opos)
{
    glm::dvec3 up = { 0, 1, 0 };
    
    rrot = glm::lookAt(rpos, opos, up);

    // Logger::logger->debug("Camera: {} {} {}\n", upos.x, upos.y, upos.z);
    // Logger::logger->debug("Object: {} {} {}\n", opos.x, opos.y, opos.z);
    // Logger::logger->debug("Rotation Matrix:\n");
    // Logger::logger->debug("{} {} {}\n", urot[0][0], urot[0][1], urot[0][2]);
    // Logger::logger->debug("{} {} {}\n", urot[1][0], urot[1][1], urot[1][2]);
    // Logger::logger->debug("{} {} {}\n", urot[2][0], urot[2][1], urot[2][2]);
}

void Camera::setRelativePosition(double phi, double theta, double dist)
{
    erot.x = phi;
    erot.y = theta;

    double sph = sin(phi), cph = cos(phi);
    double sth = sin(theta), cth = cos(theta);

    rrot = {
        { cph, sph*sth,  -sph*cth },
        { 0.0, cth,      sth      },
        { sph, -cph*sth, cph*cth  }
    };
    rpos  = { rrot[0][2]*dist, rrot[1][2]*dist, rrot[2][2]*dist };
    rdist = dist;

    // Logger::logger->debug("{} {} {} => {} {} {} dist {}\n",
    //     upos.x, upos.y, upos.z, rpos.x, rpos.y, rpos.z, rdist);
}

void Camera::dolly(double dz)
{
    double fact = std::max(1.0/rdist, 1.0/(1.0 - dz));
    setRelativePosition(erot.x, erot.y, rdist*fact);
}

void Camera::orbit(double dx, double dy)
{
    setRelativePosition(erot.x+dx*0.5, erot.y+dy*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camera::orbitPhi(double dx)
{
    setRelativePosition(erot.x+dx*0.5, erot.y, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camera::orbitTheta(double dy)
{
    setRelativePosition(erot.x, erot.y+dy*0.5, rdist);
    udir = rrot * glm::dvec3(0, 0, 1);
}

void Camera::rotate(double dx, double dy, double dz)
{
    // Update current X (Phi), Y (Theta), and Z.
    clrot.x += dx;
    clrot.y += dy;
    clrot.z += dz;

    double sx = sin(clrot.x), cx = cos(clrot.x);
    double sy = sin(clrot.y), cy = cos(clrot.y);
    double sz = sin(clrot.z), cz = cos(clrot.z);

    // rrot = {
    //     { cx*cz, sz,     -sx*cy },
    //     { -sz,   cy*cz,   sy    },
    //     { sx,   -cx*sy,   cx*cy }
    // };
}

void Camera::rotatePhi(double dPhi)
{

}

void Camera::rotateTheta(double dTheta)
{

}

void Camera::attach(Object *object, extCameraMode nMode)
{
    // Assign object (planet, vessel, etc) to target
    // as relative coordinates
    targetObject = object;

    if (modeExternal == true)
    {
        setRelativePosition(erot.x, erot.y, rdist);

    }
}

void Camera::update()
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

void Camera::processKeyboard()
{

}

// ******** Mouse Controls ********

void Camera::mouseMove(float mx, float my, int state)
{
    // // Camera rotation controls
    // if (state & ofs::mouseButton::mouseLeftButton)
    // {

    // }

    // // Orbital movement controls
    // if (state & ofs::mouseButton::mouseRightButton)
    // {
    //     double dx = mlx - mx;
    //     double dy = mly - my;
        
    //     orbit(dx * -0.005, dy * -0.005);
    // }

    mlx = mx;
    mly = my;
}

void Camera::mousePressButtonDown(float mx, float my, int state)
{

    // Reset mouse motion
    mlx = mx;
    mly = my;
}

void Camera::mousePressButtonUp(float mx, float my, int state)
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

void Camera::mouseDialWheel(float motion, int state)
{
    // int dz = motion;
    // if (modeExternal == true)
    //     dolly(-dz * 0.05);
}
