// camera.h - Visual Camera package
//
// Author:  Tim Stark
// Date:    Sep 16, 2022

#pragma once

class Camera
{
public:
    Camera(int width, int height);
    ~Camera() = default;

    inline glm::dvec3 getGlobalPosition() const    { return gpos; }
    inline glm::dvec3 getGlobalDirection() const   { return gdir; }
    inline glm::dmat3 getGlobalRotation() const    { return grot; }

    inline glm::dmat4 getViewMatrix() const        { return dmView; }
    inline glm::dmat4 getProjMatrix() const        { return dmProj; }
    inline glm::dmat4 getViewProjMatrix() const    { return dmViewProj; }

    void setSize(int w, int h);

    void update();

private:
    int width, height;

    glm::dvec3 gpos;            // Global position
    glm::dvec3 gdir;            // Global direction
    glm::dmat4 grot;            // Global rotation/orientation

    double zNear, zFar;         // Frustum Limits
    double aspect;              // Screen aspect ratio
    double fov;                 // Field of view

    glm::dmat4 dmView;         // View matrix
    glm::dmat4 dmProj;         // Projection matrix
    glm::dmat4 dmViewProj;     // View/projection matrix
};