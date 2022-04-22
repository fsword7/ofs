// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Apt 17, 2022

#include "main/core.h"
#include "engine/object.h"
#include "universe/frame.h"
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
: RigidBody("Player", objPlayer)
{
    cameras.push_back(new Camera(*this));

    frame = new PlayerFrame();
    updateFrame(frame);
}

Player::~Player()
{
    if (frame != nullptr)
        delete frame;
}

void Player::updateFrame(PlayerFrame *nFrame)
{
    lpos = nFrame->fromUniversal(upos, jdTime);
    lrot = nFrame->fromUniversal(urot, jdTime);
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
    upos = frame->toUniversal(lpos, jdTime);
    urot = frame->toUniversal(lrot, jdTime);

    // fmt::printf("To Universal: L(%lf,%lf,%lf) => U(%lf,%lf,%lf)\n",
    //     lpos.x, lpos.y, lpos.z, upos.x, upos.y, upos.z);
    // fmt::printf("              P(%lf,%lf,%lf,%lf) => Q(%lf,%lf,%lf,%lf)\n",
    //     lrot.w, lrot.x, lrot.y, lrot.z, urot.w, urot.x, urot.y, urot.z);
}

void Player::start(double tjd)
{
    // Reset julian date/time
    realTime = tjd;
    jdTime = tjd;
    deltaTime = 0;
}

void Player::update(double dt, double timeTravel)
{
    realTime += dt / SECONDS_PER_DAY;
    jdTime   += (dt / SECONDS_PER_DAY) * timeTravel;
    deltaTime = dt;

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

    // fmt::printf("Move: (%lf,%lf,%lf) <= (%lf,%lf,%lf)\n",
    //     lpos.x, lpos.y, lpos.z, tv.x, tv.y, tv.z);
    // fmt::printf("Rotate: (%lf,%lf,%lf,%lf) <= (%lf,%lf,%lf) Q(%lf,%lf,%lf,%lf)\n",
    //     lrot.w, lrot.x, lrot.y, lrot.z, av.x, av.y, av.z, wv.w, wv.x, wv.y, wv.z);

    // Updating current universal coordinates
    updateUniversal();
}

void Player::move(Object *object, double altitude, goMode mode)
{
    // vec3d_t opos = object->getuPosition(jdTime);
    // quatd_t orot;
    // vec3d_t tpos;

    // // PlanetarySystem *system;
    // // celStar *sun;

    // switch (mode)
    // {
    // case goGeoSync:
    //     orot = object->getuOrientation(jdTime);
    //     break;
    // };

    // upos = opos + glm::conjugate(orot) * vec3d_t(0, 0, altitude);
    // urot = orot;
    // lpos = frame->fromUniversal(upos, jdTime);
    // lrot = frame->fromUniversal(urot, jdTime);

    lpos = vec3d_t(0, 0, altitude);
    updateUniversal();
}

double Player::computeCoarseness(double maxCoarseness)
{
    double radius   = 1;
    double distance = glm::length(lpos);
    double altitude = distance - radius;
    double coarse   = maxCoarseness;

    if (altitude > 0 && altitude < radius)
        coarse *= std::max(0.01, altitude/radius);
    return coarse;
}

void Player::dolly(double delta)
{
    // const Object *object = frame->getCenter();
    // if (object == nullptr)
    //     return;

    vec3d_t opos = { 0, 0, 0 }; // object->getPosition(jdTime);
    double  surfaceDistance = 1; // object->getRadius();
    double  naturalDistance = surfaceDistance * 4.0;
    double  currentDistance = glm::length(lpos);

    if (currentDistance >= surfaceDistance && naturalDistance != 0)
    {
        double r = (currentDistance - surfaceDistance) / naturalDistance;
        double newDistance = surfaceDistance + naturalDistance * exp(log(r) + delta);

        lpos *= (newDistance / currentDistance);

        // Updating current universal coordinates
        updateUniversal();
    }
}

void Player::orbit(quatd_t rot)
{
    // const Object *center = frame->getCenter();
    // if (center == nullptr)
    //     return;

    double dist  = glm::length(lpos);
    quatd_t qrot = glm::normalize(lrot * rot * glm::conjugate(lrot));

    lpos = glm::normalize(glm::conjugate(qrot) * lpos) * dist;
    lrot = glm::conjugate(qrot) * lrot;

    // fmt::printf("Rotation: R(%lf,%lf,%lf,%lf) => Q(%lf,%lf,%lf,%lf)\n",
    //     rot.w, rot.x, rot.y, rot.z, qrot.w, qrot.x, qrot.y, qrot.z);
    // fmt::printf("Local:    Q(%lf,%lf,%lf,%lf) => L(%lf,%lf,%lf,%lf)\n",
    //     qrot.w, qrot.x, qrot.y, qrot.z, lrot.w, lrot.x, lrot.y, lrot.z);

    updateUniversal();
}
