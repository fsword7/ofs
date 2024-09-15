// atmo.h - Atmospheric pressure/scattering package
//
// Author:  Tim Stark
// Date:    Jul 9, 2022

#pragma once

struct atmconst_t
{
    color_t color0;         // Sky color at sea level (daytime)
    double  altLimit;       // Atomsphere atitude limit
    double  radLimit;       // Radius limit (Mean radius + atomsphere limit)
    double  hAlt;           // horizon rendering altitude

    double  p0;             // air pressure at mean radius (sea level)
    double  rho0;           // density at mean radius
    double  R;              // specific gas constant
    double  gamma;          // ratio of specific heats
    double  C;              // exponent for pressure equation
    double  O2app;          // partial pressure of oxygen
};

// Atmospheric parameters (input)
struct iatmprm_t
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
struct atmprm_t
{
    double p;           // Pressure [Pa]
    double rho;         // Density [kg/m^3]
    double T;           // Temperature [K]
};

class Atmosphere
{
public:
    Atmosphere() = default;

    static Atmosphere *create(str_t &name);

    virtual str_t getsAtmName() const = 0;
    virtual void getAtmConstants(atmconst_t &atmc) const = 0;
    virtual void getAtmParams(const iatmprm_t &in, atmprm_t &out) = 0;

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