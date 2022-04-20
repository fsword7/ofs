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

void Player::updateUniversal()
{
    upos = lpos;
    urot = lrot;
}

void Player::move(Object *object, double altitude, goMode mode)
{
    lpos = vec3d_t( 0, 0, altitude );

    updateUniversal();
}