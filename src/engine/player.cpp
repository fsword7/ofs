// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/ofsapi.h"
#include "main/timedate.h"
// #include "engine/frame.h"
#include "engine/vehicle.h"
#include "universe/celbody.h"
#include "universe/body.h"
#include "engine/player.h"

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

glm::dvec3 Camera::getPickRay(double x, double y)
{
    double s = 2.0 * tan(fov / 2.0);
    return glm::normalize(glm::dvec3(x*s, y*s, -1.0));
}

double Camera::getFieldCorrection() const
{
    return 2.0 * sfov / (ofs::degrees(fov) + sfov); 
}

// ******** Player ********

Player::Player(TimeDate *td)
: cam(SCR_WIDTH, SCR_HEIGHT), td(td)
{
    updateCamera();

    // Allocating two elevation tiles
    elevTiles.resize(2);
}

Player::~Player()
{
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

double Player::getElevation(CelestialPlanet *cbody, double lat, double lng, double alt)
{
    ElevationManager *emgr = cbody->getElevationManager();
    if (emgr == nullptr)
        return 0.0;

    int lod = 15;
    // int ilat, ilng;
    // emgr->getTileIndex(lat, lng, lod, ilat, ilng);
    // return 0.0;

    return emgr->getElevationData({lat, lng, alt}, lod, &elevTiles);   
}

void Player::attach(Object *object, cameraMode mode)
{
    tgtObject = object;

    modeCamera = mode;

    if (modeExternal)
    {
        switch (modeCamera)
        {
        case camTargetRelative:
            // Move observer to target object
            gspos = tgtObject->getuOrientation(0) * cam.rpos;
            gpos  = tgtObject->getoPosition() + gspos;
            grot  = tgtObject->getuOrientation(0) * cam.rrot;
            gqrot = grot;
            break;
        }
    }
    else
    {
        assert(tgtObject->getType() == objVehicle);
        Vehicle *vehicle = dynamic_cast<Vehicle *>(tgtObject);

        vcpos = vehicle->getCameraPosition();
        vcdir = vehicle->getCameraDirection();

        grot = tgtObject->getuOrientation(0) * cam.rrot;
        gspos = tgtObject->getuOrientation(0) * *vcpos;
        gpos = gspos + tgtObject->getuPosition(0);
    }

    updateCamera();
}

void Player::look(Object *object)
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

    CelestialPlanet *cbody = nullptr;
    double elev = 0.0;

    if (modeExternal)
    {

        if (modeCamera == camTargetRelative || modeCamera == camTargetRelative)
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

        case camTargetRelative:
            gspos = tgtObject->getuOrientation(0) * cam.rpos;
            gpos  = tgtObject->getoPosition() + gspos;
            grot  = tgtObject->getuOrientation(0) * cam.rrot;
            gqrot = grot;

            // ofsLogger->debug("TR Local Position:    ({:f}, {:f}, {:f})\n", gspos.x, gspos.y, gspos.z);
            // ofsLogger->debug("TR Global Position:   ({:f}, {:f}, {:f})\n", gpos.x, gpos.y, gpos.z);
            // ofsLogger->debug("TR Location:          {:f} {:f}\n", glm::degrees(go.lat), glm::degrees(go.lng));
            // ofsLogger->debug("TR Altitude:          {:f}\n", glm::length(gspos));

            break;

        // case camSolarSyncRelative:

        case camGroundObserver:
            {
                cbody = dynamic_cast<CelestialPlanet *>(tgtObject);
                assert(cbody != nullptr);

                elev = getElevation(cbody, go.lat, go.lng, go.alt);
                // ofsLogger->debug("Location: {:f}, {:f} -> {:f} feet\n",
                //     glm::degrees(go.lat), glm::degrees(go.lng), elev);

                // Calkculating planetocentric coordinates
                double rad = cbody->getRadius() + go.alt;
                cam.rpos = cbody->convertEquatorialToLocal(go.lat, go.lng, rad);
                gspos = cbody->getuOrientation(0) * cam.rpos;
                gpos = cbody->getoPosition() + gspos;

                // Rotate camera in local frame. Negate theta value for 
                // clockwise rotation. Points to east as default origin
                // so that adding pi/2.0 to theta value for pointing to 
                // north with zero heading.  
                cam.rrot = xRotate(go.phi) * yRotate(-go.theta + pi/2.0);

                // glm::dvec3 wv = go.av * 0.5;
                // glm::dquat dr = glm::dquat(1.0, wv.x, wv.y, wv.z) * cam.rqrot;
                // cam.rqrot = glm::normalize(cam.rqrot + dr);
                // cam.rrot = glm::mat3_cast(cam.rqrot);

                // cam.rpos -= glm::conjugate(cam.rqrot) * tv;

                grot = cam.rrot * go.R * cbody->getuOrientation(0);
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
    }
    else
    {
        // Internal camera updates

        assert(tgtObject->getType() == objVehicle);
        
        // Set global position/rotation for on the air
        grot  = tgtObject->getuOrientation(0) * cam.rrot;
        gspos = grot * (cam.rpos + *vcpos);
        gpos  = tgtObject->getoPosition() + gspos;
    }

    updateCamera();
}

// rotate camera 
void Player::rotateView(double dtheta, double dphi)
{
    if (modeExternal && modeCamera == camGroundObserver)
        rotateGroundObserver(dtheta, dphi);
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
    if (modeExternal && modeCamera != camTargetRelative)
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
    if (modeExternal && modeCamera != camTargetRelative)
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

void Player::setGroundObserver(Object *object, glm::dvec3 loc, double heading)
{
    CelestialBody *cbody = dynamic_cast<CelestialBody *>(object);
    if (cbody == nullptr)
        return;
    attach(object, camGroundObserver); 

    go.lat = glm::radians(loc.x);
    go.lng = glm::radians(loc.y);
    go.dir = glm::radians(heading);

    go.alt = loc.z;
    go.alt0 = loc.z;

    go.theta = glm::radians(heading);
    go.phi = glm::radians(0.0);

    // Clear all ground velocity controls
    go.av = { 0, 0, 0 };
    go.tv = { 0, 0, 0 };

    // Set rotation matrix for local horizon frame
    // for right-handed rule (OpenGL). Points
    // to east as origin at (0, 0).
    //
    //     |  slat  clat   0  | |  clng   0   slng |
    // R = | -clat  slat   0  | |   0     1    0   |
    //     |   0     0     1  | | -slng   0   clng |
    //
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
    // to east as origin at (0, 0).
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