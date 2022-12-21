// atomsphee.h - Atomsphere package
//
// Author:  Tim Stark
// Date:    Jul 3, 2022

class Atomsphere
{
public:
    // Atmospheric parameter structure
    struct AtmParam
    {
        double T;       // Temperature [K]
        double p;       // Pressure [Pa]
        double rho;     // Density [kg/m^3]
    };

private:
    double altLimit;    // atmospheric altitude limit
    double radLimit;    // radius limit (mean radius + altitude limit)
    double altHorizon;  // horizon rendering altitude

    // Planetary atmospheric constants
    double p0;          // pressures at MSL [Pa]
    double rho0;        // density at MSL
    double R;           // specific gas constant [J / (K Kg)]
    double gamma;       // ratio of heats [c_p/c_v]
    double C;           // exponent for pressure equation
    double O2pp;        // partial pressure of oxygen

    // Planetary atmospheric constants for rendering
    double blue;        // Blue wavelength
    double green;       // Green wavelength
    double red;         // Red wavelength

};
