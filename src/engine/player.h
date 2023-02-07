// player.h - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Feb 6, 2023

#pragma once

class Object;
class Player;

enum cameraMode {
    camGlobalFrame,

};

enum travelMode
{
    travelFreeFlight,
    travelExternal,
    travelCocpkit
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

    inline glm::dvec3 getGlobalPosition() { return rpos; }
    inline glm::dmat3 getGlobalRotation() { return rrot; }

    inline glm::dmat4 getProjMatrix() { return proj; }
    inline glm::dmat4 getViewMatrix() { return view; }

    void resize(int width, int height);

    void setPosition(const glm::dvec3 &vpos);
    void setRotation(const glm::dmat3 &vrot);
    void look(const glm::dvec3 &opos);

    void update();

    void updateProjMatrix();
    void updateViewMatrix();

private:
    int    width, height;
    double aspect;
    double fov;
    double zNear, zFar;

    glm::dvec3 rpos  = glm::dvec3(0, 0, 0);
    glm::dmat3 rrot  = glm::dmat3(1);
    double     rdist = glm::length(rpos);

    // Projection/view matrix
    glm::dmat4 proj;
    glm::dmat4 view;
};

class OFSAPI Player
{
public:
    Player();
    ~Player();

    inline bool isExternal() const              { return modeExternal; }
    inline bool isInternal() const              { return !modeExternal; }
    inline travelMode getTravelMode() const     { return modeTravel; }
    inline Camera *getCamera()                  { return &cam; }

    inline glm::dvec3 getGlobalPosition()       { return gpos; }
    inline glm::dmat3 getGlobalRotation()       { return grot; }
    
    void attach(Object *object);

    void update();

    void rotatePhi(double phi);     // X rotation
    void rotateTheta(double theta); // Y rotation

    void addPhi(double dphi);     // X rotation
    void addTheta(double dtheta); // Y rotation

    void orbit(double phi, double theta, double dist);
    void rotateView(double phi, double theta);

private:
    Camera cam;

    Object *tgtObject = nullptr;

    // Global (universal) parmeters
    glm::dvec3 gpos;
    glm::dmat3 grot;

    double ephi = 0.0;      // current phi rotation (external)
    double etheta = 0.0;    // current theta rotation (external)
    double cphi = 0.0;      // current phi rotation (free)
    double ctheta = 0.0;    // current theta rotation (free)

    bool modeExternal = true;
    travelMode modeTravel = travelFreeFlight;
    cameraMode modeCamera = camGlobalFrame;

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