// lights.h - Light enviroment package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Oct 22, 2023

#pragma once

#define MAX_LIGHTS 10

// Directed llght list
struct DirectLight
{
    glm::dvec3      spos;           // Sun position
    glm::dvec3      dObject;        // Directed object
    glm::dvec3      dEye;           // Directed eye
    color_t         color;          // Spectrum
    double          irradiance;     // Brightness
    double          asize;          // Apparent size
    bool            shadows;        // Casting shadow enable
};

// Light state for OpenGL interface
struct LightState
{
    glm::dvec3      eyeDirObject;
    glm::dvec3      eyePosObject;
    glm::dvec3      ambientColor;
    uint32_t        nLights;
    DirectLight     lights[MAX_LIGHTS];
};

// Secondary Light Source (reflected from celestial bodies)
// struct SecondaryLight
// {
//     const Celestial *body = nullptr;
//     glm::dvec3 vpos;        // viewer relative position
//     double radius;          // radius [Km]
//     double reflected;       // total irradiance from direct sources
// };