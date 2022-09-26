// camera.h - Camera package
//
// Author:  Tim Stark
// Date:    Sep 26, 2022

#pragma once

class Object;

class Camerax
{
public:
    Camerax(int w, int h);
    ~Camerax() = default;

    inline glm::dvec3 getGlobalPosition() const     { return upos; }
    inline glm::dvec3 getGlobalDirection() const    { return udir; }
    inline glm::dmat3 getGlobalRotation() const     { return urot; }

    inline glm::dvec3 getLocalPosition() const      { return lpos; }
    inline glm::dvec3 getLocalDirection() const     { return ldir; }
    inline glm::dmat3 getLocalRotation() const      { return lrot; }

    inline glm::dmat4 getViewMatrix() const         { return glm::dmat4(urot); }
    inline glm::dmat4 getProjectionMatrix() const
        { return glm::perspective(fov, aspect, zNear, zFar); }

    void resize(int w, int h);

    void orbit(double phi, double theta);
    void orbitPhi(double phi);
    void orbitTheta(double theta);

    void setRelativePosition(double phi, double theta, double dist);

    void update();

    void processMouse(int x, int y, int state);

private:
    int    width, height;
    double aspect;
    double fov;
    double zNear, zFar;

    double ePhi, eTheta;

    bool modeExternal = true;

    Object *target = nullptr;

    glm::dvec3 upos = { 0, 0, -60000 };
    glm::dvec3 udir = { 0, 0, 1 };
    glm::dmat3 urot = glm::dmat3(1);

    glm::dvec3 lpos = { 0, 0, 0 };
    glm::dvec3 ldir = { 0, 0, 1 };
    glm::dmat3 lrot = glm::dmat3(1);

    glm::dvec3 rpos;
    double     rdist;
    glm::dmat3 rrot;
};