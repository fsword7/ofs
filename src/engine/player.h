// player.h - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#pragma once

class Object;
class Vehicle;
class Celestial;
class CelestialPlanet;
class ElevationTile;
class Player;
class pSystem;

enum cameraMode {
    camGlobalFrame,
    camTargetRelative,
    camTargetUnlocked,
    camTargetSync,
    camGroundObserver,
    camPersonalObserver,    // Personal observer/player - walking
    camCockpit
};

enum cameraAction {
    camNormal,
    camRecenter,
    camRecenterCont
};

enum travelMode
{
    travelFreeFlight,
    travelExternal,
    travelCockpit
};

struct GroundObserver
{
    double lat, lng;    // Latitude/Longtiude location
    double dir;         // ground direction (compass)
    double alt;         // Altitude
    double alt0;        // Altitude at sea level
    double elev;        // Elevation from sea level
    double theta, phi;  // Camera direction at local horizon frame
    double panSpeed;    // speed at ground for movement controls (m/s)

    double vcofs;       // cockpit offset from AGL level

    glm::dvec3 av;      // angular velocity control
    glm::dvec3 tv;      // travel velocity control

    glm::dmat3 R;       // local horizon frame
    glm::dquat Q;       //    quaternion
};

class OFSAPI Camera
{
    friend class Player;

public:
    Camera(int width, int height);
    ~Camera() = default;

    inline int getWidth()               { return width; }
    inline int getHeight()              { return height; }

    inline double getAspect()           { return aspect; }
    inline double getFOV()              { return fov; }
    inline double getAperature()        { return tan(fov); }
    inline double getScale()            { return h05 / tan(fov); }
    inline glm::vec2 getClip()          { return glm::vec2(zNear, zFar); }

    inline glm::dvec3 getGlobalPosition() { return rpos; }
    inline glm::dmat3 getGlobalRotation() { return rrot; }

    inline glm::dmat4 getProjMatrix()       { return proj; }
    inline glm::dmat4 getViewMatrix()       { return view; }
    inline glm::dmat4 getProjViewMatrix()   { return proj*view; }

    double getPixelSize() const;
    void mapMouse(float wx, float wy, float &vx, float &vy) const;
    glm::dvec3 getPickRay(double x, double y) const;
    double getFieldCorrection() const;

    void resize(int width, int height);

    void setPosition(const glm::dvec3 &vpos);
    void setRotation(const glm::dmat3 &vrot);
    void look(const glm::dvec3 &opos);

private:
    int    width, height;
    int    w05, h05;
    int    wx = 0, wy = 0;
    double aspect;
    double fov;
    double zNear, zFar;

    const double sfov = 45.0; // standard FOV - 45 degrees

    glm::dvec3 rpos  = glm::dvec3(0, 0, 0);
    glm::dmat3 rrot  = glm::dmat3(1);
    glm::dquat rqrot = glm::dquat(1, 0, 0, 0);

    double rdist = glm::length(rpos);

    // Projection/view matrix
    glm::dmat4 proj;
    glm::dmat4 view;
};

class OFSAPI Player
{
public:
    Player(TimeDate *td, int width, int height);
    Player(json &config, TimeDate *td, int width, int height);
    ~Player();

    void configure(json &config);

    inline bool isExternal() const              { return modeExternal; }
    inline bool isInternal() const              { return !modeExternal; }
    inline travelMode getTravelMode() const     { return modeTravel; }
    inline cameraMode getCameraMode() const     { return modeCamera; }
    inline Camera *getCamera()                  { return &cam; }
    inline TimeDate *getTimeDate()              { return td; }

    inline glm::dmat4 getProjMatrix() const     { return proj; }
    inline glm::dmat4 getViewMatrix() const     { return view; }

    inline glm::dvec3 getPosition() const           { return gpos; }
    inline glm::dmat3 getRotation() const           { return grot; }
    inline glm::dquat getqRotation() const          { return gqrot; }
    inline glm::dvec3 getrPosition() const          { return cam.rpos; }
    inline glm::dmat3 getrRotation() const          { return cam.rrot; }

    inline glm::dvec3 getAngularControl() const         { return av; }
    inline glm::dvec3 getTravelControl() const          { return tv; }
    inline glm::dvec3 getGroundAngularControl() const   { return go.av; }
    inline glm::dvec3 getGroundTravelControl() const    { return go.tv; }

    inline double getCockpitPhi() const                 { return cphi; }
    inline double getCockpitTheta() const               { return ctheta; }
    inline double getCockpitPhi0() const                { return cphi0; }
    inline double getCockpitTheta0() const              { return ctheta0; }
    inline double getCockpitTilt() const                { return ctilt; }
    inline double getCOckpitTilt0() const               { return ctilt0; }

    inline Vehicle *getVehicleTarget() const            { return vehObject; }
    inline Celestial *getReferenceObject()              { return tgtObject; }
    inline const Celestial *getReferenceObject() const  { return tgtObject; }
    inline Celestial *getSyncObject()                   { return syncObject; }

    inline void setAngularControl(glm::dvec3 _av)       { av = _av; }
    inline void setTravelControl(glm::dvec3 _tv)        { tv = _tv; }
    inline void setGroundAngularControl(glm::dvec3 av)  { go.av = av; }
    inline void setGroundTravelControl(glm::dvec3 tv)   { go.tv = tv; }

    inline pSystem *getSystem() const                   { return system; }
    inline void setSystem(pSystem *sys)                 { system = sys; }

    double computeCoarseness(double maxCoarseness);

    void attach(Celestial *object, cameraMode mode = camGlobalFrame, Celestial *sobject = nullptr);

    double getGroundElevation(CelestialPlanet *cbody, double lat, double lng);

    // void updateProjMatrix();
    // void updateViewMatrix();
    void updateCamera();

    void update(const TimeDate &td);

    void look(Celestial *object);

    void dolly(double dz);
    void orbit(const glm::dquat &drot);
    void orbit(double phi, double theta, double dist);
    void rotateView(double theta, double phi);

    // void setGroundObserver(Object *object, double lng, double lat, double heading, double alt);
    void setGroundObserver(Celestial *object, glm::dvec3 loc, double heading);
    void shiftGroundObserver(glm::dvec3 dm, double dh);
    void rotateGroundObserver(double dtheta, double dphi);

    // Personal observer - walking, etc.
    void setPersonalObserver(Celestial *object, glm::dvec3 loc, double heading);
    void shiftPersonalObserver(glm::dvec3 dm, double dh);
    void rotatePersonalObserver(double dtheta, double dphi);

    // Cockpit function calls
    void rotateCockpit(double phi, double theta, double tilt = 0.0);
    void setDefaultCockpitDir(const glm::dvec3 &dir = {0, 0, 1}, double tilt = 0.0);
    void setCockpitDir(double phi, double theta, double tilt = 0.0);
    void resetCockpitDir(double phi, double theta, bool smooth = false);
    void resetCockpitDir(bool smooth = false);

private:
    Camera cam;
    // PlayerFrame *frame = nullptr;

    pSystem   *system = nullptr;
    Celestial *tgtObject = nullptr;
    Celestial *syncObject = nullptr;
    Celestial *focusObject = nullptr;
    Vehicle   *vehObject = nullptr;

    // Global (universal) parmeters
    glm::dvec3 gspos = {};   // Relative to target in global coordinates
    glm::dvec3 gpos  = {};
    glm::dvec3 gdir  = { 0, 0, 1 };    // Direction
    glm::dmat3 grot  = {};
    glm::dquat gqrot = {};
    glm::dmat3 osrot = {};   // solar/object-sync reference frame

    glm::dvec3 *vcpos = nullptr;    // virtual cockpit position
    glm::dvec3 *vcdir = nullptr;    // virtual cockpit direction

    double cphi, ctheta, ctilt;     // current cockpit phi/theta
    double tcphi, tctheta, tctilt;
    double cphi0, ctheta0, ctilt0;  // default cockpit phi/theta

    glm::dmat3 rrot0;           // default cockpit rotation
    glm::dvec3 eyeofs, eyeofs0; // eye offset [vehicle frame]
    bool isCenterDir = false;   // center direction path [vehicle frame]

    double ephi = 0.0;      // current phi rotation (external)
    double etheta = 0.0;    // current theta rotation (external)
    // double vcphi = 0.0;     // current phi rotation (cockpit)
    // double vctheta = 0.0;   // current theta rotation (cockpit)
    // double tcphi = 0.0;     // current smooth phi rotation (cockpit)
    // double tctheta = 0.0;   // current smooth theta rotation (cockpit)
    // double cphi = 0.0;      // current phi rotation (free)
    // double ctheta = 0.0;    // current theta rotation (free)

    GroundObserver go;      // Ground observer - watching vehicle from tower or ground.
    GroundObserver pgo;     // Personal observer - walking around.

    // Rotation: X = Phi, Y = Theta, Z = distance
    glm::dvec3 orot;        // external camera from object frame

    glm::dvec3 av = { 0, 0, 0 };    // angular velocity control
    glm::dvec3 tv = { 0, 0, 0 };    // travel velocity control

    bool modeExternal = true;
    travelMode modeTravel  = travelFreeFlight;
    cameraMode modeCamera  = camGlobalFrame;
    cameraAction camAction = camNormal;
 
    TimeDate *td = nullptr;

    // Projection/view matrix
    glm::dmat4 proj;
    glm::dmat4 view;

    std::vector<ElevationTile> elevTiles;
};

// class Camera
// {
// public:
//     Camera(int w, int h);
//     ~Camera() = default;

//     inline int getWidth() const                     { return width; }
//     inline int getHeight() const                    { return height; }

//     inline glm::dvec3 getGlobalPosition() const     { return upos; }
//     inline glm::dvec3 getGlobalDirection() const    { return udir; }
//     inline glm::dmat3 getGlobalRotation() const     { return urot; }

//     inline glm::dvec3 getLocalPosition() const      { return lpos; }
//     inline glm::dvec3 getLocalDirection() const     { return ldir; }
//     inline glm::dmat3 getLocalRotation() const      { return lrot; }

//     inline double getFOV() const                    { return fov; }
//     inline double getAspect() const                 { return aspect; }
//     inline double getTanAp() const                  { return tan(fov); }

//     inline glm::dmat4 getViewMatrix() const         { return glm::dmat4(urot); }
//     inline glm::dmat4 getProjectionMatrix() const   { return proj; }

//     void resize(int w, int h);
//     void reset();

//     void updateProjectionMatrix();

//     void setPosition(const glm::dvec3 &pos);
//     void setRotation(const glm::dmat3 &rot);

//     void look(const glm::dvec3 &opos);

//     void dolly(double dz);
    
//     void orbit(double phi, double theta);
//     void orbitPhi(double phi);
//     void orbitTheta(double theta);

//     void rotate(double dx, double dy, double dz);
//     void rotatePhi(double phi);
//     void rotateTheta(double theta);

//     void setRelativePosition(double phi, double theta, double dist);

//     void attach(Object *object, extCameraMode mode);

//     void update();

//     void processKeyboard();

//     // Mouse contols
//     void mouseMove(float mx, float my, int state);
//     void mousePressButtonDown(float mx, float my, int state);
//     void mousePressButtonUp(float mx, float my, int state);
//     void mouseDialWheel(float motion, int state);

// private:
//     int    width, height;
//     double aspect;
//     double fov;
//     double zNear, zFar;

//     int mlx, mly;   // Last moouse motion

//     // Rotation: X = Phi, Y = Theta
//     glm::dvec3 erot;        // rotation [radians - object frame]
//     glm::dvec3 clrot;       // local camera rotation [radians]

//     bool modeExternal = true;
//     extCameraMode mode = modeGlobalFrame;

//     Object *targetObject = nullptr;

//     glm::dmat4 proj = glm::dmat4(1.0);
//     glm::dmat4 view = glm::dmat4(1.0);

//     // Universe parameters [global frame]
//     glm::dvec3 upos = { 0, 0, 0 };
//     glm::dvec3 udir = { 0, 0, 1 };
//     glm::dmat3 urot = glm::dmat3(1);

//     // Local parameters [planet frame]
//     glm::dvec3 lpos = { 0, 0, 0 };
//     glm::dvec3 ldir = { 0, 0, 1 };
//     glm::dmat3 lrot = glm::dmat3(1);

//     // Vessel parameters [vessel frame]
//     glm::dvec3 vpos = { 0, 0, 0 };
//     glm::dvec3 vdir = { 0, 0, 1 };
//     glm::dmat3 vrot = glm::dmat3(1);

//     // Cockpit parameters [vessel frame]
//     glm::dvec3 cpos = { 0, 0, 0 };
//     glm::dvec3 cdir = { 0, 0, 1 };
//     glm::dmat3 crot = glm::dmat3(1);

//     glm::dvec3 rpos;
//     double     rdist;
//     glm::dmat3 rrot;
// };