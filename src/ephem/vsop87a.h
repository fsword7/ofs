// vsop87a.h - VSOP87 Ephemeris data package
//
// Author:  Tim Stark
// Date:    Nov 3, 2022

#define VSOP_PARAMS     6   // XYZ position/velocity parameters
#define VSOP_MAXALPHA   5

class OrbitVSOP87
{
public:
    OrbitVSOP87(ObjectHandle object);
    virtual ~OrbitVSOP87();
    
    virtual uint16_t getOrbitData(double mjd, uint16_t req, double *ret) = 0;

protected:
    void setSeries(char series);
    void init();
    void load(cstr_t &name);
    
    void getEphemeris(double mjd, double *ret);

    int index[3];
    double terms[3];

    double a0;      // semi-major axis [AU]

    int nalpha;
    int16_t fmtFlags = 0;

private:
    char sid;

};