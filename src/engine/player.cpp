// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/ofsapi.h"
#include "main/timedate.h"
#include "main/app.h"
// #include "engine/frame.h"
#include "engine/vehicle/vehicle.h"
#include "universe/celbody.h"
#include "universe/body.h"
#include "universe/universe.h"
#include "engine/player.h"
#include "utils/json.h"

// ******** Camera ********

Camera::Camera(int width, int height)
{
    fov   = glm::radians(SCR_FOV);
    zNear = 0.0001;
    zFar  = 1e24; // Known universe size

    resize(width, height);
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
    rqrot = glm::quat_cast(rrot);

    // Logger::logger->debug("Camera: {} {} {}\n", upos.x, upos.y, upos.z);
    // Logger::logger->debug("Object: {} {} {}\n", opos.x, opos.y, opos.z);
    // Logger::logger->debug("Rotation Matrix:\n");
    // Logger::logger->debug("{} {} {}\n", urot[0][0], urot[0][1], urot[0][2]);
    // Logger::logger->debug("{} {} {}\n", urot[1][0], urot[1][1], urot[1][2]);
    // Logger::logger->debug("{} {} {}\n", urot[2][0], urot[2][1], urot[2][2]);
}

double Camera::getPixelSize() const
{
    return 2.0 * tan(fov / 2.0) / height;
}

void Camera::mapMouse(float mx, float my, float &vx, float &vy) const
{
    float w = width;
    float h = height;

    // Convert to [-1:1]
    vx = ((mx - wx) / w) - 0.5;
    vy = ((h - (my - wy)) / h) - 0.5;
}

glm::dvec3 Camera::getPickRay(double x, double y) const
{
    double s = 2.0 * tan(fov / 2.0);
    return glm::normalize(glm::dvec3(x*s, y*s, -1.0));
}

double Camera::getFieldCorrection() const
{
    return 2.0 * sfov / (ofs::degrees(fov) + sfov); 
}

// ******** Player ********

Player::Player(TimeDate *td, int width, int height)
: cam(width, height), td(td)
{
    updateCamera();

    // Allocating two elevation tiles
    elevTiles.resize(2);
}

Player::Player(json &config, TimeDate *td, int width, int height)
: cam(width, height), td(td)
{


    updateCamera();

    // Allocating two elevation tiles
    elevTiles.resize(2);
}

Player::~Player()
{
}

void Player::configure(json &config)
{
    Universe *univ = ofsAppCore->getUniverse();

    Celestial *primary = nullptr, *secondary = nullptr;

    if (config["mode"].is_object()) {
        json &modes = config["mode"];
        str_t modeType = myjson::getString<str_t>(modes, "type");
        cameraMode camMode;

        if (modeType == "target-sync")
            camMode = camTargetSync;
        else if (modeType == "target-relative")
            camMode = camTargetRelative;
        else if (modeType == "target-unlocked")
            camMode = camTargetUnlocked;
        else if (modeType == "global-frame")
            camMode = camGlobalFrame;
        else if (modeType == "ground-observer")
            camMode = camGroundObserver;
        else if (modeType == "personal-observer")
            camMode = camPersonalObserver;
        else if (modeType == "cockpit")
            camMode = camCockpit;
        else {
            ofsLogger->error("JSON: Unknown camera type: {} - aborted\n", modeType);
            return;
        }

        str_t primaryTarget, secondaryTarget;
        glm::dvec3 ploc, rpos, rdir;
        double dir;
        double rad, elev = 0.0;
        double scale;

        switch (camMode) {
        case camPersonalObserver:
            primaryTarget = myjson::getString<str_t>(modes, "target");
            if (!primaryTarget.empty())
                primary = univ->findPath(primaryTarget);

            if (!modes.contains("location") || !modes.contains("heading")) {
                ofsLogger->error("JSON: Required location and heading - aborted\n");
                return;
            }
            ploc = myjson::getFloatArray<glm::dvec3, double>(modes, "location");
            dir = myjson::getFloat<double>(modes, "heading");

            setPersonalObserver(primary, ploc, dir);
            break;

        case camTargetRelative:
        case camTargetSync:
        case camTargetUnlocked:
        case camGlobalFrame:
            primaryTarget = myjson::getString<str_t>(modes, "target");
            if (!primaryTarget.empty())
                primary = univ->findPath(primaryTarget);
            if (camMode == camTargetSync) {
                secondaryTarget = myjson::getString<str_t>(modes, "sync-target");
                if (!secondaryTarget.empty())
                    secondary = univ->findPath(secondaryTarget);
            }

            rad = primary->getRadius();
            elev = 0.01;

            scale = myjson::getFloat<double>(modes, "scale", 1.0);

            if (modes.contains("location")) {
                ploc = myjson::getFloatArray<glm::dvec3, double>(modes, "location");

                ploc.x = ofs::radians(ploc.x);
                ploc.y = ofs::radians(ploc.y);
                ploc.z += (ploc.z < rad) ? rad : 0;

                rpos = primary->convertEquatorialToLocal(ploc);
            } else if (modes.contains("position")) {
                rpos = myjson::getFloatArray<glm::dvec3, double>(modes, "position");

                if (glm::length(rpos) < rad)
                    rpos = glm::normalize(rpos) *
                        ((elev != 0) ? (rad + elev) : (rad * scale));
            } else if (modes.contains("direction")) {
                rdir = myjson::getFloatArray<glm::dvec3, double>(modes, "direction");

                rpos = rdir * (rad * scale);
            } else {
                ofsLogger->error("JSON: Required location, position or direction - aborted\n");
                return;    
            }
        
            cam.setPosition(rpos);

            modeExternal = true;
            attach(primary, camMode, secondary);
            break;
        
        case camCockpit:
            primaryTarget = myjson::getString<str_t>(modes, "target");
            if (!primaryTarget.empty())
                primary = univ->findVehicle(primaryTarget);
            
            Vehicle *veh = dynamic_cast<Vehicle *>(primary);
            assert(veh != nullptr);
            vcpos = veh->getCameraPosition();
            vcdir = veh->getCameraDirection();

            modeExternal = false;
            attach(primary, camCockpit);
            break;
        }
    }

    str_t focusTarget = myjson::getString<str_t>(config, "focus");
    if (!focusTarget.empty())
        focusObject = univ->findPath(focusTarget);
    if (focusObject == nullptr)
        focusObject = primary;
    updateCamera();
}

double Player::computeCoarseness(double maxCoarseness)
{
    double radius   = 1;
    double distance = glm::length(cam.rpos);
    double altitude = distance - radius;
    double coarse   = maxCoarseness;

    if (altitude > 0 && altitude < radius)
        coarse *= std::max(0.01, altitude/radius);
    return coarse;
}

double Player::getGroundElevation(CelestialPlanet *cbody, double lat, double lng)
{
    double elev = 0.0;
    ElevationManager *emgr = cbody->getElevationManager();
    if (emgr != nullptr)
    {
        int rlod = int(32.0 - log(std::max(pgo.alt0, 0.1))*(1.0 / log(2.0)));
        elev = emgr->getElevationData({lat, lng, pgo.alt0}, rlod, &elevTiles);
    }
    return elev / 1000.0; 
}

void Player::attach(Celestial *object, cameraMode mode, Celestial *sobject)
{
    tgtObject = object;
    if (sobject != nullptr && mode == camTargetSync)
        syncObject = sobject;

    modeCamera = mode;

    // if (modeExternal)
    // {
    //     switch (modeCamera)
    //     {
    //     case camTargetUnlocked:
    //         // Move observer to target object but
    //         // not locked to target rotation
    //         gspos = cam.rpos;
    //         gpos  = tgtObject->getgPosition() + gspos;
    //         grot  = cam.rrot;
    //         break;

    //     case camTargetRelative:
    //         // Move observer to target object
    //         {
    //             glm::dmat3 trot = tgtObject->getgRotation();
    //             gspos = cam.rpos * trot;
    //             gpos  = tgtObject->getgPosition() + gspos;
    //             grot  = cam.rrot * trot;
    //             gqrot = grot;
    //         }
    //         break;

    //     case camTargetSync:
    //         {
    //             // Move observer to target object
    //             // with solar/object sync
    //             glm::dvec3 opos, tpos;
    //             opos = syncObject->getgPosition();
    //             tpos = tgtObject->getgPosition();
    //             osrot  = glm::lookAt(opos, tpos, { 0, 1, 0});
    //             gspos = cam.rpos * osrot;
    //             gpos  = tpos + gspos;
    //             grot  = cam.rrot * osrot;
    //         }
    //         break;
    //     }
    // }
    // else
    // {
    //     assert(tgtObject->getType() == objVehicle);
    //     Vehicle *veh = dynamic_cast<Vehicle *>(tgtObject);
    
    //     switch (modeCamera) {
    //     case camCockpit:
    //         vcpos = veh->getCameraPosition();
    //         vcdir = veh->getCameraDirection();

    //         grot = tgtObject->getgRotation() * cam.rrot;
    //         gspos = tgtObject->getgRotation() * *vcpos;
    //         gpos = gspos + tgtObject->getgPosition();
    //         break;
    //     }
    // }

    // Assign player to planetary system
    pSystem *psys = tgtObject->getPlanetarySystem();
    setSystem(psys);

    // updateCamera();
}

void Player::look(Celestial *object)
{
    if (object == nullptr)
        return;
    
    glm::dvec3 up = { 0, 1, 0 };
    
    glm::dvec3 opos = { 0, 0, 0 }; //object->getoPosition();
    cam.rrot  = glm::lookAt(cam.rpos, opos, up);
    cam.rqrot = cam.rrot;

    updateCamera();
}

void Player::updateCamera()
{
    cam.proj = glm::perspective(cam.fov, cam.aspect, cam.zNear, cam.zFar);
    cam.view = grot;
}

void Player::update(const TimeDate &td)
{
    if (tgtObject->s1.bUpdates == false)
        return;

    CelestialPlanet *cbody = nullptr;
    double elev = 0.0;

    if (modeExternal)
    {

        if (modeCamera == camTargetRelative || modeCamera == camTargetUnlocked ||
            modeCamera == camTargetSync)
        {        
            // free travel mode
            // Update current position and attitude in local reference frame
            // applying angular velocity to rotation quaternion in local space.
            //
            //      dq/dt = q * w * t/2
            //      where w = (0, x, y, z)
            //

            glm::dvec3 wv = av * 0.5;
            glm::dquat dr = glm::dquat(1.0, wv.x, wv.y, wv.z) * cam.rqrot;
            cam.rqrot = glm::normalize(cam.rqrot + dr);
            cam.rrot = glm::mat3_cast(cam.rqrot);

            cam.rpos -= glm::conjugate(cam.rqrot) * tv;

            // ofsLogger->debug("Angular Velocity\n");
            // ofsLogger->debug("WV: {} {} {}\n", wv.x, wv.y, wv.z);
            // ofsLogger->debug("DR: {} {} {} {}\n", dr.w, dr.x, dr.y, dr.z);
            // ofsLogger->debug("Q:  {} {} {} {}\n", cam.rqrot.w, cam.rqrot.x, cam.rqrot.y, cam.rqrot.z);

            // ofsLogger->debug("Rotation matrix:\n");
            // ofsLogger->debug("{} {} {}\n", cam.rrot[0][0], cam.rrot[0][1], cam.rrot[0][2]);
            // ofsLogger->debug("{} {} {}\n", cam.rrot[1][0], cam.rrot[1][1], cam.rrot[1][2]);
            // ofsLogger->debug("{} {} {}\n", cam.rrot[2][0], cam.rrot[2][1], cam.rrot[2][2]);
        }

        // External camera updates
        switch (modeCamera)
        {
        case camGlobalFrame:
            gpos = cam.rpos;
            grot = cam.rrot;
            break;

        case camTargetUnlocked:
            gspos = cam.rpos;
            gpos  = tgtObject->s1.pos + gspos;
            grot  = cam.rrot;
            break;

        case camTargetRelative:
            {
                glm::dmat3 trot = tgtObject->s1.R;
                gspos = cam.rpos * trot;
                gpos  = tgtObject->s1.pos + gspos;
                grot  = cam.rrot * trot;
                gqrot = grot;
            }
            // ofsLogger->debug("TR Local Position:    ({:f}, {:f}, {:f})\n", gspos.x, gspos.y, gspos.z);
            // ofsLogger->debug("TR Global Position:   ({:f}, {:f}, {:f})\n", gpos.x, gpos.y, gpos.z);
            // ofsLogger->debug("TR Location:          {:f} {:f}\n", glm::degrees(go.lat), glm::degrees(go.lng));
            // ofsLogger->debug("TR Altitude:          {:f}\n", glm::length(gspos));

            break;

        case camTargetSync:
            {
                glm::dvec3 opos, tpos;

                opos = syncObject->s1.pos;
                tpos = tgtObject->s1.pos;

                // ofsLogger->info("{}: S0 {}, {}, {}\n", tgtObject->getsName(),
                //     tgtObject->s0.pos.x, tgtObject->s0.pos.y, tgtObject->s0.pos.z);
                // ofsLogger->info("{}: S1 {}, {}, {} Flag: {}\n", tgtObject->getsName(),
                //     tgtObject->s1.pos.x, tgtObject->s1.pos.y, tgtObject->s1.pos.z, tgtObject->s1.bUpdates);
               
                osrot = glm::lookAt(opos, tpos, { 0, 1, 0});
                gspos = cam.rpos * osrot;
                gpos  = tpos + gspos;
                grot  = cam.rrot * osrot;

                // ofsLogger->info("{}: O {}, {}, {}\n", tgtObject->getsName(),
                //     opos.x, opos.y, opos.z);
                // ofsLogger->info("{}: T {}, {}, {}\n", tgtObject->getsName(),
                //     tpos.x, tpos.y, tpos.z);
                // ofsLogger->info("{}: S {}, {}, {}\n", tgtObject->getsName(),
                //     gspos.x, gspos.y, gspos.z);
                // ofsLogger->info("{}: P {}, {}, {}\n", tgtObject->getsName(),
                //     gpos.x, gpos.y, gpos.z);
            }
            break;

        case camPersonalObserver:
            {
                cbody = dynamic_cast<CelestialPlanet *>(tgtObject);
                assert(cbody != nullptr);

                pgo.alt0 = cbody->getRadius();
                pgo.elev = getGroundElevation(cbody, pgo.lat, pgo.lng);
                pgo.alt  = pgo.alt0 + pgo.elev;
                // ofsLogger->debug("Location: {:f}, {:f} -> {:f} feet\n",
                //     glm::degrees(go.lat), glm::degrees(go.lng), elev);

                // Calkculating planetocentric coordinates
                double vcalt = pgo.alt + pgo.vcofs;
                cam.rpos = cbody->convertEquatorialToLocal(pgo.lat, pgo.lng, vcalt);
                gspos = cam.rpos * cbody->s1.R;
                gpos = cbody->s1.pos + gspos;

                // Rotate camera in local frame. Negate theta value for 
                // clockwise rotation. Points to east as default origin
                // so that using heading rotation (ofs::hRotate).  
                cam.rrot = ofs::xRotate(pgo.phi) * ofs::hRotate(pgo.theta);

                // glm::dvec3 wv = go.av * 0.5;
                // glm::dquat dr = glm::dquat(1.0, wv.x, wv.y, wv.z) * cam.rqrot;
                // cam.rqrot = glm::normalize(cam.rqrot + dr);
                // cam.rrot = glm::mat3_cast(cam.rqrot);

                // cam.rpos -= glm::conjugate(cam.rqrot) * tv;

                grot = cam.rrot * pgo.R * cbody->s1.R;
                gqrot = grot;
 
                // ofsLogger->debug("R = {:f} {:f} {:f}\n", go.R[0][0], go.R[0][1], go.R[0][2]);
                // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[1][0], go.R[1][1], go.R[1][2]);
                // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[2][0], go.R[2][1], go.R[2][2]);

                // ofsLogger->debug("GO Local Position:    ({:f}, {:f}, {:f})\n", gspos.x, gspos.y, gspos.z);
                // ofsLogger->debug("GO Global Position:   ({:f}, {:f}, {:f})\n", gpos.x, gpos.y, gpos.z);
                // ofsLogger->debug("GO Location:          {:f} {:f}\n", glm::degrees(go.lat), glm::degrees(go.lng));
                // ofsLogger->debug("GO Altitude:          {:f}\n", rad);
            }
            break;
        };

        // focus on object at request
        if (focusObject != nullptr)
        {
            look(focusObject);
            focusObject = nullptr;
        }
    }
    else
    {
        // Internal camera updates (in ship)
        assert(tgtObject->getType() == objVehicle);

        switch (modeCamera)
        {
        case camCockpit:
            // Set global position/rotation for on the air
            grot  = tgtObject->s1.R * cam.rrot;
            gspos = grot * (cam.rpos + *vcpos);
            gpos  = tgtObject->s1.pos + gspos;

            // ofsLogger->info("{}: T {}, {}, {}\n", tgtObject->getsName(),
            //     tgtObject->s1.pos.x, tgtObject->s1.pos.y, tgtObject->s1.pos.z);
            // ofsLogger->info("{}: VC {}, {}, {}\n", tgtObject->getsName(),
            //     vcpos->x, vcpos->y, vcpos->z);
            // ofsLogger->info("{}: S {}, {}, {}\n", tgtObject->getsName(),
            //     gspos.x, gspos.y, gspos.z);
            // ofsLogger->info("{}: P {}, {}, {}\n", tgtObject->getsName(),
            //     gpos.x, gpos.y, gpos.z);

            break;
        }

    }

    updateCamera();
}

// rotate camera 
void Player::rotateView(double dtheta, double dphi)
{
    if (modeExternal) {
        if (modeCamera == camGroundObserver)
            rotateGroundObserver(dtheta, dphi);
        else if (modeCamera == camPersonalObserver)
            rotatePersonalObserver(dtheta, dphi);
    }
}

void Player::orbit(double phi, double theta, double dist)
{
    // If reference object is not attached,
    // do nothing and return.
    if (tgtObject == nullptr)
        return;
    

}

void Player::orbit(const glm::dquat &drot)
{
    if (tgtObject == nullptr)
        return;
    if (modeExternal && modeCamera == camGroundObserver)
        return;

    glm::dvec3 vpos = cam.rpos;

    double vdist = glm::length(vpos);
    glm::dquat qrot = glm::conjugate(cam.rqrot) * drot * cam.rqrot;
    vpos = glm::conjugate(qrot) * vpos;
    vpos = glm::normalize(vpos) * vdist;

    cam.rqrot = cam.rqrot * qrot;
    cam.rrot  = glm::mat3_cast(cam.rqrot);
    cam.rpos  = vpos;
}

void Player::dolly(double dz)
{
    if (tgtObject == nullptr)
        return;
    if (modeExternal && modeCamera == camGroundObserver)
        return;

    // double fact = std::max(1.0/opos.z, 1.0/(1.0 - dz));
    // orbit(opos.x, opos.y, opos.z*fact);

    glm::dvec3 vpos = cam.rpos;
    double sdist = tgtObject->getRadius();  // surface radius
    double vdist = glm::length(vpos);

    if (vdist > sdist)
    {
        double agl = (vdist - sdist) / sdist;
        double ndist = sdist + sdist * exp(log(agl) + dz);

        vpos *= ndist / vdist;
        cam.rpos = vpos;
        // Logger::logger->debug("Distance: {} <- {} / {}\n", glm::length(cam.rpos), ndist, cdist);
    }

    updateCamera();
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

void Player::setGroundObserver(Celestial *object, glm::dvec3 loc, double heading)
{
    if (object == nullptr)
        return;
    attach(object, camGroundObserver); 

    go.lat = glm::radians(loc.x);
    go.lng = glm::radians(loc.y);
    go.dir = glm::radians(heading);
    go.vcofs = loc.z;

    go.theta = glm::radians(heading);
    go.phi = glm::radians(0.0);

    // Clear all ground velocity controls
    go.av = { 0, 0, 0 };
    go.tv = { 0, 0, 0 };

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0) so that using
    // heading rotation (ofs::hRotate).
    //
    //     / |  slat -clat   0  | |  clng   0  -slng | \ T
    // R = | |  clat  slat   0  | |   0     1    0   | |
    //     \ |   0     0     1  | |  slng   0   clng | /

    double clat = cos(go.lat), slat = sin(go.lat);
    double clng = cos(go.lng), slng = sin(go.lng);
    go.R = { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };
    go.Q = go.R;

    // cam.rqrot = xqRotate(-go.phi) * yqRotate(go.theta - pi/2.0);
    // cam.rrot  = glm::mat3_cast(cam.rqrot);

    // ofsLogger->debug("R = {:f} {:f} {:f}\n", go.R[0][0], go.R[0][1], go.R[0][2]);
    // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[1][0], go.R[1][1], go.R[1][2]);
    // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[2][0], go.R[2][1], go.R[2][2]);
    // ofsLogger->debug("Q = {:f} {:f} {:f} {:f}\n", go.Q.w, go.Q.x, go.Q.y, go.Q.z);
}

void Player::shiftGroundObserver(glm::dvec3 dm, double dh)
{
    if (modeExternal && modeCamera != camGroundObserver)
        return;

    // Rotate movement at the direction of camera view.
    dm = cam.rrot * dm;

    // Updating ground observer
    go.lat += dm.z;
    go.lng += dm.x;
    go.alt += dh;
    go.alt0 += dh;
    
    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0) so that using
    // heading rotation (ofs::hRotate).
    //
    //     / |  slat -clat   0  | |  clng   0  -slng | \ T
    // R = | |  clat  slat   0  | |   0     1    0   | |
    //     \ |   0     0     1  | |  slng   0   clng | /
    //
    double clat = cos(go.lat), slat = sin(go.lat);
    double clng = cos(go.lng), slng = sin(go.lng);
    go.R = { slat*clng,  clat*clng, slng,
            -clat,       slat,      0,
            -slat*slng, -clat*slng, clng };
    go.Q = go.R;
}

void Player::rotateGroundObserver(double dtheta, double dphi)
{
    if (modeExternal && modeCamera != camGroundObserver)
        return;
    go.theta += dtheta;
    go.phi   += dphi;
}


void Player::setPersonalObserver(Celestial *object, glm::dvec3 loc, double heading)
{
    if (object == nullptr)
        return;
    attach(object, camPersonalObserver); 

    pgo.lat = glm::radians(loc.x);
    pgo.lng = glm::radians(loc.y);
    pgo.dir = glm::radians(heading);
    pgo.vcofs = loc.z;

    pgo.theta = glm::radians(heading);
    pgo.phi = glm::radians(0.0);

    // Clear all ground velocity controls
    pgo.av = { 0, 0, 0 };
    pgo.tv = { 0, 0, 0 };

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0) so that using
    // heading rotation (ofs::hRotate).
    //
    //     / |  slat -clat   0  | |  clng   0  -slng | \ T
    // R = | |  clat  slat   0  | |   0     1    0   | |
    //     \ |   0     0     1  | |  slng   0   clng | /
    //
    double clat = cos(pgo.lat), slat = sin(pgo.lat);
    double clng = cos(pgo.lng), slng = sin(pgo.lng);
    pgo.R = { slat*clng,  clat*clng, slng,
             -clat,       slat,      0,
             -slat*slng, -clat*slng, clng };
    pgo.Q = pgo.R;

    // cam.rqrot = xqRotate(-go.phi) * yqRotate(go.theta - pi/2.0);
    // cam.rrot  = glm::mat3_cast(cam.rqrot);

    // ofsLogger->debug("R = {:f} {:f} {:f}\n", go.R[0][0], go.R[0][1], go.R[0][2]);
    // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[1][0], go.R[1][1], go.R[1][2]);
    // ofsLogger->debug("    {:f} {:f} {:f}\n", go.R[2][0], go.R[2][1], go.R[2][2]);
    // ofsLogger->debug("Q = {:f} {:f} {:f} {:f}\n", go.Q.w, go.Q.x, go.Q.y, go.Q.z);
}

void Player::shiftPersonalObserver(glm::dvec3 dm, double dh)
{
    if (modeExternal && modeCamera != camPersonalObserver)
        return;

    // Rotate movement at the direction of camera view.
    dm = cam.rrot * dm;

    // Updating personal observer
    pgo.lat += dm.z;
    pgo.lng += dm.x;
    pgo.alt += dh;
    pgo.alt0 += dh;
    
    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    double clat = cos(pgo.lat), slat = sin(pgo.lat);
    double clng = cos(pgo.lng), slng = sin(pgo.lng);
    pgo.R = { slat*clng,  clat*clng, slng,
             -clat,       slat,      0,
             -slat*slng, -clat*slng, clng };
    pgo.Q = pgo.R;
}

void Player::rotatePersonalObserver(double dtheta, double dphi)
{
    if (modeExternal && modeCamera != camPersonalObserver)
        return;
    pgo.theta += dtheta;
    pgo.phi   += dphi;
}
