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

    vec3d_t ray = vec3d_t(vx * s * aspect, vy * s, -1.0);
    ray.normalize();

    return ray;
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
        Logger::getLogger()->fatal("Failed to set new player framme - aborted\n");
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
}

void Player::setTravelVelocity(vec3d_t _tv)
{
    tv = _tv;
}

void Player::updateUniversal()
{
    upos = frame->toUniversal(lpos, jdTime);
    urot = frame->toUniversal(lrot, jdTime);
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

        vec3d_t wv = av * 0.5;
        quatd_t dr = quatd_t(0.5, wv.x(), wv.y(), wv.z()) * lrot;
        lrot = quatd_t(dr.coeffs() + dt * dr.coeffs());
        lrot.normalize();
    
        // Logger::getLogger()->debug("WV   {:.6f} {:.6f} {:.6f}\n", wv.x(), wv.y(), wv.z());
        // Logger::getLogger()->debug("QWV  {:.6f} {:.6f} {:.6f} {:.6f}\n", qwv.w(), qwv.x(), qwv.y(), qwv.z());
        // Logger::getLogger()->debug("DR   {:.6f} {:.6f} {:.6f} {:.6f}\n", dr.w(), dr.x(), dr.y(), dr.z());
        // Logger::getLogger()->debug("LROT {:.6f} {:.6f} {:.6f} {:.6f}\n", lrot.w(), lrot.x(), lrot.y(), lrot.z());

        lpos -= (lrot.conjugate() * tv) * dt;

    }

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
        orot = quatd_t(Eigen::AngleAxis<double>(J2000Obliquity, vec3d_t::UnitX()));
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
        orot = ofs::lookAt(opos, tpos, vec3d_t(0, 1, 0));
        break;

    case goBackHelioSync:
        if (object->getType() == objCelestialBody)
        {
            system = dynamic_cast<celBody *>(object)->getInSystem();
            if (system != nullptr)
                sun = system->getStar();
        }
        tpos = sun->getuPosition(jdTime);
        orot = ofs::lookAt(tpos, opos, vec3d_t(0, 1, 0));
        break;
    }

    upos = opos + orot.conjugate() * vec3d_t(0, 0, -altitude);
    urot = orot.conjugate();
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

    urot = ofs::lookAt(upos, opos, up);
    lrot = frame->fromUniversal(urot, jdTime);
}

double Player::computeCoarseness(double maxCoarseness)
{
    double radius   = 1;
    double distance = lpos.norm();
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
    double  currentDistance = lpos.norm();

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
    // If tracking object reference not set, assign center object in local
    // reference frame as default. If center object is not available,
    // do nothing and return.
    if (trackingObject == nullptr)
    {
        if (frame != nullptr)
            trackingObject = frame->getCenter();
        if (trackingObject == nullptr)
            return;
    }

    // Determine central position of object (planet, vessel, etc)
    vec3d_t cpos = frame->fromUniversal(trackingObject->getuPosition(jdTime), jdTime);
    vec3d_t vpos = lpos - cpos;

    double vdist = vpos.norm();
    quatd_t qrot = lrot.conjugate() * rot * lrot;
    vpos = qrot.conjugate() * vpos;
    vpos = vpos.normalized() * vdist;

    lrot = lrot * qrot;
    lpos = cpos + vpos;

    updateUniversal();
}
