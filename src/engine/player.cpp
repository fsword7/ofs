// player.cpp - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#define OFSAPI_SERVER_BUILD

#include "main/core.h"
#include "api/ofsapi.h"
#include "main/timedate.h"
// #include "engine/frame.h"
#include "universe/celbody.h"
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

void Camera::updateProjMatrix()
{
    proj = glm::perspective(fov, aspect, zNear, zFar);
    // proj = glm::perspectiveLH(fov, aspect, zNear, zFar);
}

void Camera::updateViewMatrix()
{
    view = rrot;
    // view = glm::transpose(rrot);
}

void Camera::update()
{
    updateProjMatrix();
    updateViewMatrix();
}

// ******** Player ********

Player::Player(TimeDate *td)
: cam(SCR_WIDTH, SCR_HEIGHT), td(td)
{
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

void Player::attach(Object *object, cameraMode mode)
{
    tgtObject = object;

    modeCamera = mode;

    if (modeExternal)
    {
        // Move observer to target object
        gspos = tgtObject->getuOrientation(0) * cam.rpos;
        gpos  = tgtObject->getoPosition() + gspos;
        grot  = tgtObject->getuOrientation(0) * cam.rrot;
        gqrot = grot;
    }

    cam.update();
}

void Player::look(Object *object)
{
    if (object == nullptr)
        return;
    
    glm::dvec3 up = { 0, 1, 0 };
    
    glm::dvec3 opos = { 0, 0, 0 }; //object->getoPosition();
    cam.rrot  = glm::lookAt(cam.rpos, opos, up);
    cam.rqrot = cam.rrot;

    cam.update();
}

void Player::update(const TimeDate &td)
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

    CelestialBody *cbody = nullptr;

    glm::dvec3 wv = av * 0.5;
    glm::dquat dr = glm::dquat(1.0, wv.x, wv.y, -wv.z) * cam.rqrot;
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

        case camGroundObserver:
            cbody = dynamic_cast<CelestialBody *>(tgtObject);
            assert(cbody != nullptr);

            double rad = cbody->getRadius() + go.alt;
            cam.rpos = cbody->convertEquatorialToLocal(go.lat, go.lng, rad);
            gspos = cbody->getuOrientation(0) * cam.rpos;
            gpos = cbody->getoPosition() + gspos;

            // gpos = cbody->convertEquatorialToGlobal(go.lat, go.lng, rad);
            // gspos = gpos - cbody->getoPosition();
            
            grot = cam.rrot;
            gqrot = grot;

            ofsLogger->debug("GO Local Position:    ({:f}, {:f}, {:f})\n", gspos.x, gspos.y, gspos.z);
            ofsLogger->debug("GO Global Position:   ({:f}, {:f}, {:f})\n", gpos.x, gpos.y, gpos.z);
            ofsLogger->debug("GO Location:          {:f} {:f}\n", glm::degrees(go.lat), glm::degrees(go.lng));
            ofsLogger->debug("GO Altitude:          {:f}\n", rad);

            // double rad = tgtObject->getRadius() + (go.alt / 1000.0);
            // // gpos = tgtObject->convertEquatorialToGlobal(go.lng, go.lat, rad);
            // gspos = gpos - tgtObject->getoPosition();
            // gdir = glm::normalize(tgtObject->getoPosition()-gpos);
            // glm::dvec3 hdir = tmul(go.R, tmul(tgtObject->getuOrientation(0), gdir));
            // if (fabs(hdir.y) < 0.999999)
            // {
            //     go.theta = asin(hdir.y);
            //     go.phi = atan2(-hdir.x, hdir.z);
            // }
            // else
            // {
            //     go.theta = (hdir.y > 0) ? pi/2.0 : -pi/2.0;
            //     go.phi = 0.0;
            // }
            // double sinph = sin(go.phi), cosph = cos(go.phi);
            // double sinth = sin(go.theta), costh = cos(go.theta);
            // cam.rrot = { cosph,     sinph*sinth,    -sinph*costh,
            //              0.0,       costh,          sinth,
            //              sinph,     -cosph*sinth,   cosph*costh };
            // grot = tgtObject->getuOrientation(0) * go.R * cam.rrot;
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

void Player::orbit(const glm::dquat &drot)
{
    if (tgtObject == nullptr)
        return;
    if (modeExternal != camTargetRelative)
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
    if (modeExternal != camTargetRelative)
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

    cam.update();
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

void Player::setGroundMode(Object *object, double lng, double lat, double heading, double alt)
{
    if (dynamic_cast<CelestialBody *>(object) == nullptr)
        return;
    attach(object, camGroundObserver); 

    go.lng = lng;
    go.lat = lat;
    go.alt = alt;
    go.dir = heading;
    
    double clng = cos(lng), slng = sin(lng);
    double clat = cos(lat), slat = sin(lat);

    go.R = { clng*slat, clng*clat, -slng,
            -clat,      slat,       0,
             slng*slat, slng*clat,  clng };

}

void Player::setGroundMode(Object *object, glm::dvec3 loc, double heading)
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

    double clat = cos(go.lat), slat = sin(go.lat);
    double clng = cos(go.lng), slng = sin(go.lng);
    double cdir = cos(go.dir), sdir = sin(go.dir);

    go.R = { clng*slat, clng*clat, -slng,
            -clat,      slat,       0,
             slng*slat, slng*clat,  clng };

    double rad = cbody->getRadius() + go.alt;
    cam.rpos = cbody->convertEquatorialToLocal(go.lat, go.lng, rad);
    gspos = tgtObject->getuOrientation(0) * cam.rpos;
    gpos  = tgtObject->getoPosition() + gspos;
    grot  = tgtObject->getuOrientation(0) * cam.rrot;
    gqrot = grot;

    // ofsLogger->debug("GO: ({} {}) {} {}\n", glm::degrees(go.lat), glm::degrees(go.lng),
    //     go.alt, glm::degrees(go.dir));
}