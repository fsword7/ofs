// observer.h - Observer (player/camera) package
//
// Author:  Tim Stark
// Date:    Oct 13, 2023

#pragma once


enum modeType
{
    obsGlobalFrame,
    obsBodyFixed
};

enum cameraMode {
    camGlobalFrame,     // Global frame reference
    camTargetRelative,  // Body fixed frame reference
    camSyncObject
};

enum travelMode
{
    travelFreeFlight,
    travelExternal,
    travelCocpkit
};

class Observer
{
public:
    Observer(int width, int height);
    ~Observer() = default;

    inline int getWidth()               { return width; }
    inline int getHeight()              { return height; }

    inline double getAspect()           { return aspect; }
    inline double getFOV()              { return fov; }
    inline double getAperature()        { return tan(fov); }
    inline glm::vec2 getViewClip()      { return glm::vec2(zNear, zFar); }

    void resize(int width, int height);

    void attach(Object *object, cameraMode mode);

    void update(const TimeDate &td);

private:
    // View parameters
    int    width, height;
    double aspect;
    double fov;
    double zNear, zFar;

    bool extMode = true;

    travelMode modeTravel = travelFreeFlight;
    cameraMode modeCamera = camGlobalFrame;

    Object *tgtObject = nullptr;
    Object *syncObject = nullptr;

    // Movement controls (external view)
    glm::dvec3 av = { 0, 0, 0 };    // angular velocity control
    glm::dvec3 tv = { 0, 0, 0 };    // travel velocity control

    // Relative parameters to reference of target object
    glm::dvec3 rpos  = { 0, 0, 0 };
    glm::dmat3 rrot  = glm::dmat3(1);
    glm::dquat rqrot = { 1, 0, 0, 0 };

    // Global parameters
    glm::dvec3 gspos;
    glm::dvec3 gpos;
    glm::dmat3 grot;
    glm::dquat gqrot;

};