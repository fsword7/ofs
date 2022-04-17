// player.h - Player/Observer Package
//
// Author:  Tim Stark
// Date:    Apt 17, 2022

#pragma once

#include "engine/object.h"

class Player;

class Camera
{
public:
    Camera(Player &player) : player(player)
    { }
    ~Camera() = default;

    inline vec3d_t getuPosition() const;
    inline quatd_t getuOrientation() const;
    inline vec3d_t getlPosition() const;
    inline quatd_t getlOrientation() const;

    inline int getWidth() const     { return width; }
    inline int getHeight() const    { return height; }
    inline double getAspect() const { return aspect; }
    inline double getFOV() const    { return fov; }
    inline double getTanAp() const  { return tan(fov); }

    void setViewport(int w, int h);

private:
    Player &player;

    vec3d_t rpos = { 0, 0, 0 };     // Relative position [player frame]
    quatd_t rrot = { 1, 0, 0, 0};   // Relative orientation [player frame]

    uint32_t width = 1, height = 1;
    double   aspect = double(width) / double(height);
    double   fov = glm::radians(SCR_FOV);
};

class Player : public Object
{
public:
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
    ~Player();

    vec3d_t getuPosition()    { return upos; }
    quatd_t getuOrientation() { return urot; }
    vec3d_t getlPosition()    { return lpos; }
    quatd_t getlOrientation() { return lrot; }

    void move(Object *object, double altitude, goMode mode);

private:
    std::vector<Camera *> cameras;

    vec3d_t  upos = { 0, 0, 0 };
    quatd_t  urot = { 1, 0, 0, 0 };
    vec3d_t  lpos = { 0, 0, 0 };
    quatd_t  lrot = { 1, 0, 0, 0 };
};