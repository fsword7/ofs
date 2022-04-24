// lights.h - Light enviroment package for OpenGL interface
//
// Author:  Tim Stark
// Date:    Apr 4, 2022

#pragma once

#define MAX_LIGHTS 8

// Directed llght list
struct DirectLight
{
    vec3d_t     spos;           // Sun position
    vec3d_t     dObject;        // Directed object
    vec3d_t     dEye;           // Directed eye
    color_t     color;          // Spectrum
    double      irradiance;     // Brightness
    double      asize;          // Apparent size
    bool        shadows;        // Casting shadow enable
};

// Light state for OpenGL interface
struct LightState
{
    vec3d_t         eyeDirObject;
    vec3d_t         eyePosObject;
    vec3d_t         ambientColor;
    uint32_t        nLights;
    DirectLight     lights[MAX_LIGHTS];
};