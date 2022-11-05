// vsop87a.h - VSOP87 Ephemeris data package
//
// Author:  Tim Stark
// Date:    Nov 3, 2022

#define VSOP_PARAMS     6   // XYZ position/velocity parameters
#define VSOP_MAXALPHA   5

class VSOP87Orbit
{
public:
    VSOP87Orbit(ObjectHandle object);
    ~VSOP87Orbit();

    void getEphemeris(double mjd, double *ret);

protected:
    void setSeries(char series);
    void init();
    void load(cstr_t &name);

    int index[3];
    double terms[3];

    char sid;
    int16_t fmtFlags = 0;

};