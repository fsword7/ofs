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
    fov = glm::radians(SCR_FOV);
    zNear = 1.0;
    zFar = 1e10;

    resize(width, height);
}

void Camera::resize(int w, int h)
{
    width  = w;
    height = h;
    aspect = (double)width / (double)height;

    updateProjMatrix();
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
    rqrot = glm::quat_cast(rrot);

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
        // // free travel mode
        // // Update current position and attitude in local reference frame
        // // applying angular velocity to rotation quaternion in local space.
        // //
        // //      dq/dt = q * w * t/2
        // //      where w = (0, x, y, z)
        // //

        // vec3d_t wv = av * 0.5;
        // quatd_t dr = quatd_t(1.0, wv.x(), wv.y(), wv.z()) * lrot;
        // lrot = quatd_t(dr.coeffs() + dt * dr.coeffs());
        // lrot.normalize();
    
        // lpos -= (lrot.conjugate() * tv) * dt;

    glm::dvec3 wv = av * 0.5;
    glm::dquat dr = glm::dquat(1.0, wv.x, wv.y, wv.z) * cam.rqrot;
    cam.rqrot = glm::normalize(cam.rqrot + dr);
    cam.rrot = glm::mat3_cast(cam.rqrot);

    cam.rpos -= glm::conjugate(cam.rqrot) * tv;

    // Logger::logger->debug("Angular Velocity\n");
    // Logger::logger->debug("WV: {} {} {}\n", wv.x, wv.y, wv.z);
    // Logger::logger->debug("DR: {} {} {} {}\n", dr.w, dr.x, dr.y, dr.z);
    // Logger::logger->debug("Q:  {} {} {} {}\n", cam.rqrot.w, cam.rqrot.x, cam.rqrot.y, cam.rqrot.z);

    // Logger::logger->debug("Rotation matrix:\n");
    // Logger::logger->debug("{} {} {}\n", cam.rrot[0][0], cam.rrot[0][1], cam.rrot[0][2]);
    // Logger::logger->debug("{} {} {}\n", cam.rrot[1][0], cam.rrot[1][1], cam.rrot[1][2]);
    // Logger::logger->debug("{} {} {}\n", cam.rrot[2][0], cam.rrot[2][1], cam.rrot[2][2]);

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

    cam.update();
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