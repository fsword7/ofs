// camera.h - Camera package
//
// Author:  Tim Stark
// Date:    Sep 26, 2022

#pragma once

class Object;

enum extCameraMode {
    modeGlobalFrame,

};

enum travelMode
{
    travelFreeFlight,
    travelExternal,
    travelCocpkit
};

class Camerax
{
public:
    Camerax(int w, int h);
    ~Camerax() = default;

    inline int getWidth() const                     { return width; }
    inline int getHeight() const                    { return height; }

    inline glm::dvec3 getGlobalPosition() const     { return upos; }
    inline glm::dvec3 getGlobalDirection() const    { return udir; }
    inline glm::dmat3 getGlobalRotation() const     { return urot; }

    inline glm::dvec3 getLocalPosition() const      { return lpos; }
    inline glm::dvec3 getLocalDirection() const     { return ldir; }
    inline glm::dmat3 getLocalRotation() const      { return lrot; }

    inline double getFOV() const                    { return fov; }
    inline double getAspect() const                 { return aspect; }
    inline double getTanAp() const                  { return tan(fov); }

    inline glm::dmat4 getViewMatrix() const         { return glm::dmat4(urot); }
    inline glm::dmat4 getProjectionMatrix() const
        { return glm::perspective(fov, aspect, zNear, zFar); }

    void resize(int w, int h);
    void reset();


    void setPosition(const glm::dvec3 &pos);
    void setRotation(const glm::dmat3 &rot);

    void look(const glm::dvec3 &opos);

    void dolly(double dz);
    
    void orbit(double phi, double theta);
    void orbitPhi(double phi);
    void orbitTheta(double theta);

    void rotate(double dx, double dy, double dz);
    void rotatePhi(double phi);
    void rotateTheta(double theta);

    void setRelativePosition(double phi, double theta, double dist);

    void attach(Object *object, extCameraMode mode);

    void update();

    void processKeyboard();

    // Mouse contols
    void mouseMove(float mx, float my, int state);
    void mousePressButtonDown(float mx, float my, int state);
    void mousePressButtonUp(float mx, float my, int state);
    void mouseDialWheel(float motion, int state);

private:
    int    width, height;
    double aspect;
    double fov;
    double zNear, zFar;

    int mlx, mly;   // Last moouse motion

    // Rotation: X = Phi, Y = Theta
    glm::dvec3 erot;        // rotation [radians - object frame]
    glm::dvec3 clrot;       // local camera rotation [radians]

    bool modeExternal = true;
    extCameraMode mode = modeGlobalFrame;

    Object *targetObject = nullptr;

    // Universe parameters [global frame]
    glm::dvec3 upos = { 0, 0, 0 };
    glm::dvec3 udir = { 0, 0, 1 };
    glm::dmat3 urot = glm::dmat3(1);

    // Local parameters [planet frame]
    glm::dvec3 lpos = { 0, 0, 0 };
    glm::dvec3 ldir = { 0, 0, 1 };
    glm::dmat3 lrot = glm::dmat3(1);

    // Vessel parameters [vessel frame]
    glm::dvec3 vpos = { 0, 0, 0 };
    glm::dvec3 vdir = { 0, 0, 1 };
    glm::dmat3 vrot = glm::dmat3(1);

    // Cockpit parameters [vessel frame]
    glm::dvec3 cpos = { 0, 0, 0 };
    glm::dvec3 cdir = { 0, 0, 1 };
    glm::dmat3 crot = glm::dmat3(1);

    glm::dvec3 rpos;
    double     rdist;
    glm::dmat3 rrot;
};