// atmo.h - Atmospheric pressure/scattering package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#pragma once

class Atomsphere
{
public:
    // Atmospheric parameters (input)
    struct inAtmParam
    {
        double alt;         // Altitude [km]
        double lat;         // Latitude [radians]
        double lng;         // Longtitude [radians]
        // double ap;          // Magnetic index
        // double flux;        // Current flux
        // double fluxa;       // Average flux
        uint32_t flags;     // Parameter flags
    };

    // Atmospheric parameters (results)
    struct AtmParam
    {
        double p;           // Pressure [Pa]
        double rho;         // Density [kg/m^3]
        double T;           // Temperature [K]
    };

protected:
    double altLimit;        // Altitutde limit [km]
    double radLimit;        // Radius limit [km]
    double horizonLimit;    // rendering altitude

    // Pressure contant parameters 
    double p0;              // Pressure at MSL [Pa]
    double rho0;            // Density at MSL
    double R;               // Gas contant [J/(K kg)]
    double gamma;           // Ratio of heats (c_p/c_v)
    double C;               // Pressure equation exponent
    double O2pp;            // Partial pressure of oxygen
    
    // Scattering parameters
    color_t atmColor;       // Atmospheric color
    glm::dvec3 atmWave;        // Wavelength

};