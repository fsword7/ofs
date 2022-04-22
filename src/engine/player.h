// player.h - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Apt 17, 2022

#pragma once

#include "engine/rigidbody.h"

class Player;
class PlayerFrame;

class Camera
{
public:
    Camera(Player &player) : player(player)
    { }
    ~Camera() = default;

    inline int getWidth() const     { return width; }
    inline int getHeight() const    { return height; }
    inline double getAspect() const { return aspect; }
    inline double getFOV() const    { return fov; }
    inline double getTanAp() const  { return tan(fov); }
    
    vec3d_t getuPosition() const;
    quatd_t getuOrientation() const;
    vec3d_t getlPosition() const;
    quatd_t getlOrientation() const;

    void setViewport(int w, int h);

private:
    Player &player;

    vec3d_t rpos = { 0, 0, 0 };     // Relative position [player frame]
    quatd_t rrot = { 1, 0, 0, 0};   // Relative orientation [player frame]

    uint32_t width = 1, height = 1;
    double   aspect = double(width) / double(height);
    double   fov = glm::radians(SCR_FOV);
};

class Player : public RigidBody
{
public:
    enum travelMode
    {
        tvFreeMode
    };

    enum followMode
    {
        fwEcliptic,
        fwGeoSync,
        fwHelioSync
    };

    enum goMode
    {
        goEcliptic,
        goGeoSync,
        goFrontHelioSync,
        goBackHelioSync
    };

    Player();
    virtual ~Player();

    inline Camera *getCamera(int idx = 0) const
    { 
        return (idx >= 0 && idx < cameras.size() ? cameras[idx] : nullptr);
    }

    inline vec3d_t getuPosition() const     { return upos; }
    inline quatd_t getuOrientation() const  { return urot; }
    inline vec3d_t getlPosition() const     { return lpos; }
    inline quatd_t getlOrientation() const  { return lrot; }

    inline vec3d_t getAngularVelocity()  { return av; }
    inline vec3d_t getTravelVelocity()   { return tv; }
    inline double  getJulianTime()       { return jdTime; }

    void updateFrame(PlayerFrame *nFrame);

    void setAngularVelocity(vec3d_t av);
    void setTravelVelocity(vec3d_t tv);

    void updateUniversal();
    void start(double tjd);
    void update(double dt, double timeTravel);

    void move(Object *object, double altitude, goMode mode);

    double computeCoarseness(double maxCoarseness);
    void dolly(double delta);
    void orbit(quatd_t rot);

private:
    PlayerFrame *frame = nullptr;

    std::vector<Camera *> cameras;
    
    travelMode mode = tvFreeMode;

    vec3d_t  upos = { 0, 0, 0 };
    quatd_t  urot = { 1, 0, 0, 0 };
    vec3d_t  lpos = { 0, 0, 0 };
    quatd_t  lrot = { 1, 0, 0, 0 };
    
    // Movement control
    vec3d_t av = { 0, 0, 0 };   // angular velocity control
    quatd_t wv = { 1, 0, 0, 0}; //   quaternion control
    vec3d_t tv = { 0, 0, 0 };   // travel velocity control

    double  realTime;
    double  jdTime;
    double  deltaTime;
};