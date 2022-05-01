// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Apt 17, 2022

#include "main/core.h"
#include "engine/object.h"
#include "universe/frame.h"
#include "universe/body.h"
#include "universe/star.h"
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

vec3d_t Camera::getPickRay(float vx, float vy) const
{
    float s = float(2.0 * tan(fov / 2.0));

    return glm::normalize(vec3d_t(vx * s * aspect, vy * s, -1.0));
}

// ******** Player Reference Frame ********

PlayerFrame::PlayerFrame()
{
    frame = create(csUniversal);
}

PlayerFrame::PlayerFrame(coordType cs, Object *center, Object *target)
: type(cs)
{
    frame = create(cs, center, target);
}

PlayerFrame::~PlayerFrame()
{
    if (frame != nullptr)
        frame->release();
}

vec3d_t PlayerFrame::fromUniversal(vec3d_t upos, double tjd)
{
    return frame != nullptr ? frame->fromUniversal(upos, tjd) : upos;
}

quatd_t PlayerFrame::fromUniversal(quatd_t urot, double tjd)
{
    return frame != nullptr ? frame->fromUniversal(urot, tjd) : urot;
}

vec3d_t PlayerFrame::toUniversal(vec3d_t lpos, double tjd)
{
    return frame != nullptr ? frame->toUniversal(lpos, tjd) : lpos;
}

quatd_t PlayerFrame::toUniversal(quatd_t lrot, double tjd)
{
    return frame != nullptr ? frame->toUniversal(lrot, tjd) : lrot;
}

Frame *PlayerFrame::create(coordType csType, Object *center, Object *target)
{
    switch (csType)
    {
    case csEcliptical:
        return new J2000EclipticFrame(center);
    case csEquatorial:
        return new J2000EquatorFrame(center);
    case csBodyFixed:
        return new BodyFixedFrame(center, center);
    case csBodyMeanEquator:
        return new BodyMeanEquatorFrame(center, center);
    case csObjectSync:
        return new ObjectSyncFrame(center, target);
    case csUniversal:
    default:
        return new J2000EclipticFrame(nullptr);
    }
    return nullptr;
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

void Player::setFrame(PlayerFrame::coordType cs, Object *center, Object *target)
{
    PlayerFrame *nFrame = new PlayerFrame(cs, center, target);
    if (nFrame == nullptr)
    {
        fmt::printf("Failed to set new player framme - aborted\n");
        return;
    }

    if (frame != nullptr)
        delete frame;
    updateFrame(nFrame);
    frame = nFrame;
}

void Player::updateFrame(PlayerFrame *nFrame)
{
    lpos = nFrame->fromUniversal(upos, jdTime);
    lrot = nFrame->fromUniversal(urot, jdTime);
}

vec3d_t Player::getPickRay(float vx, float vy)
{
    if (cameras.size() > 0)
        return cameras[0]->getPickRay(vx, vy);
    return { 0, 0, 0 };
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
        // lpos -= glm::conjugate(lrot) * tv * dt;
        lpos -= (lrot * tv) * dt;
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
    vec3d_t opos = object->getuPosition(jdTime);
    quatd_t orot;
    vec3d_t tpos;

    PlanetarySystem *system;
    celStar *sun;

    switch (mode)
    {
    case goEquartorial:
        orot = quatd_t(vec3d_t(J2000Obliquity, 0, 0));
        break;

    case goBodyFixed:
        orot = object->getuOrientation(jdTime);
        break;

    case goFrontHelioSync:
        if (object->getType() == objCelestialBody)
        {
            system = dynamic_cast<celBody *>(object)->getInSystem();
            if (system != nullptr)
                sun = system->getStar();
        }
        tpos = sun->getuPosition(jdTime);
        orot = glm::lookAt(opos, tpos, vec3d_t(0, 1, 0));
        break;

    case goBackHelioSync:
        if (object->getType() == objCelestialBody)
        {
            system = dynamic_cast<celBody *>(object)->getInSystem();
            if (system != nullptr)
                sun = system->getStar();
        }
        tpos = sun->getuPosition(jdTime);
        orot = glm::lookAt(tpos, opos, vec3d_t(0, 1, 0));
        break;
    }

    upos = opos + glm::conjugate(orot) * vec3d_t(0, 0, -altitude);
    urot = glm::conjugate(orot);
    lpos = frame->fromUniversal(upos, jdTime);
    lrot = frame->fromUniversal(urot, jdTime);
}

void Player::follow(Object *object, followMode mode)
{
    PlanetarySystem *system;
    celStar *sun;

    switch (mode)
    {
    case fwEquatorial:
        setFrame(PlayerFrame::csEquatorial, object);
        break;
    case fwBodyFixed:
        setFrame(PlayerFrame::csBodyFixed, object);
        break;
    case fwHelioSync:
        if (object->getType() == objCelestialBody)
        {
            system = dynamic_cast<celBody *>(object)->getInSystem();
            if (system != nullptr)
                sun = system->getStar();
        }
        setFrame(PlayerFrame::csObjectSync, object, sun);
        break;
    case fwEcliptic:
    default:
        setFrame(PlayerFrame::csEcliptical, object);
        break;
    }
}

void Player::look(Object *object)
{
    vec3d_t opos = object->getuPosition(jdTime);
    vec3d_t up = vec3d_t(0, 1, 0);

    urot = glm::lookAt(upos, opos, up);
    lrot = frame->fromUniversal(urot, jdTime);
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

    // quatd_t qrot = glm::normalize(glm::conjugate(lrot) * rot * lrot);
    // lpos = glm::normalize(qrot * lpos) * dist;
    // lrot = lrot * qrot;

    // fmt::printf("Rotation: R(%lf,%lf,%lf,%lf) => Q(%lf,%lf,%lf,%lf)\n",
    //     rot.w, rot.x, rot.y, rot.z, qrot.w, qrot.x, qrot.y, qrot.z);
    // fmt::printf("Local:    Q(%lf,%lf,%lf,%lf) => L(%lf,%lf,%lf,%lf)\n",
    //     qrot.w, qrot.x, qrot.y, qrot.z, lrot.w, lrot.x, lrot.y, lrot.z);

    updateUniversal();
}
