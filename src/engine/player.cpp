// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Apt 17, 2022

#include "main/core.h"
#include "engine/object.h"
#include "engine/player.h"

// ******** Camera ********

vec3d_t Camera::getuPosition() const
{
    return player.getuPosition() + rpos;
}

quatd_t Camera::getuOrientation() const
{
    return player.getuOrientation() * rrot;
}

vec3d_t Camera::getlPosition() const
{
    return player.getlPosition() + rpos;
}

quatd_t Camera::getlOrientation() const
{
    return player.getlOrientation() * rrot;
}

void Camera::setViewport(int w, int h)
{
    width  = w;
    height = h;
    aspect = double(width) / double(height);
}

// ******** Player ********

Player::Player()
: Object("Player", objPlayer)
{
    cameras.push_back(new Camera(*this));

    // frame = new PlayerFrame();
    // updateFame(frame);
}

Player::~Player()
{
}

void Player::setAngularVelocity(vec3d_t _av)
{
    av = _av;
    wv = quatd_t(0, av.x, av.y, av.z);
}

void Player::setTravelVelocity(vec3d_t _tv)
{
    tv = _tv;
}

void Player::updateUniversal()
{
    upos = lpos;
    urot = lrot;
}

void Player::update(double dt, double timeTravel)
{
    // realTime += dt / SECONDS_PER_DAY;
    // jdTime   += (dt / SECONDS_PER_DAY) * timeTravel;
    // deltaTime = dt;

    if (mode == tvFreeMode)
    {
        // free travel mode
        // Update current position and attitude in local reference frame
        // applying angular velocity to rotation quaternion in local space.
        //
        //      dq/dt = q * w * t/2
        //      where w = (0, x, y, z)
        //
        lrot += lrot * wv * (dt / 2.0);
        lrot  = glm::normalize(lrot);
        lpos -= glm::conjugate(lrot) * tv * dt;
    }

    // Updating current universal coordinates
    updateUniversal();
}

void Player::move(Object *object, double altitude, goMode mode)
{
    lpos = vec3d_t( 0, 0, altitude );

    updateUniversal();
}