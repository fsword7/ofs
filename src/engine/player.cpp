// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/ofsapi.h"
#include "engine/object.h"
#include "engine/player.h"

// ******** Camera ********

Camera::Camera(int width, int height)
{
    resize(width, height);
    fov = glm::radians(SCR_FOV);
    zNear = 1.0;
    zFar = 1e10;
}

void Camera::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = (double)width / (double)height;
}

void Camera::setPosition(const glm::dvec3 &vpos)
{
    rpos  = vpos;
    rdist = glm::length(rpos);
}

void Camera::setRotation(const glm::dmat3 &vrot)
{
    rrot = vrot;
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

void Camera::updateProjMatrix()
{
    proj = ofs::perspective(fov, aspect, zNear, zFar);
}

void Camera::updateViewMatrix()
{
    view = rrot;
}

void Camera::update()
{
    updateProjMatrix();
    updateViewMatrix();
}

// ******** Player ********

Player::Player()
: cam(SCR_WIDTH, SCR_HEIGHT)
{
}

Player::~Player()
{
}

void Player::attach(Object *object)
{
    tgtObject = object;
}


void Player::update()
{

    if (modeExternal)
    {
        // External camera updates
        switch (modeCamera)
        {
        case camGlobalFrame:
            gpos = cam.rpos;
            grot = cam.rrot;
            break;
        };
    }
    else
    {
        // Internal camera updates
    }
}

// rotate camera 
void Player::rotateView(double phi, double theta)
{

}

void Player::orbit(double phi, double theta, double dist)
{
    // If reference object is not attached,
    // do nothing and return.
    if (tgtObject == nullptr)
        return;
    

}

// X (phi) rotation
void Player::addPhi(double dphi)
{

}

// Y (theta) rotation
void Player::addTheta(double dtheta)
{

}

// X (phi) rotation
void Player::rotatePhi(double phi)
{

}

// Y (theta) rotation
void Player::rotateTheta(double theta)
{

}