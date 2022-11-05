// celbody.h - Celestial body package
//
// Author:  Tim Stark
// Date:    Nov 3, 2022


// Ephemeris data - format bitflags
#define EPHEM_TRUEPOS   0x0001  // True position
#define EPHEM_TRUEVEL   0x0002  // True velocity
#define EPHEM_BARYPOS   0x0004  // Barycentric position
#define EPHEM_BARYVEL   0x0008  // Barycentric velocity

#define EPHEM_RECT      0x0010  // Rectangular data format
#define EPHEM_POLAR     0x0020  // Spherical data format
#define EPHEM_PARENT    0x0040  // Computed in the barycentric of body's system