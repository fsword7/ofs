// vsop87.h - VSOP87 Ephemeris data package
//
// Author:  Tim Stark
// Date:    Nov 3, 2022

#include "ephem/ephemeris.h"

#define VSOP_PARAMS     6   // XYZ position/velocity parameters
#define VSOP_MAXALPHA   5
struct vsop87_t
{
    double a, b, c;
};

struct vsop87s_t
{
    vsop87s_t(vsop87_t *terms, int nTerms)
    : terms(terms), nTerms(nTerms)
    { }

    int nTerms;
    vsop87_t *terms;
};

struct vsop87p_t
{
    vsop87p_t(char type, cstr_t &name, vsop87s_t *sA, int nA, vsop87s_t *sB, int nB, vsop87s_t *sC, int nC)
    : groups{sA, sB, sC}, alpha{nA, nB, nC}, name(name), type(type)
    { }

    cstr_t     name;
    char       type;
    int        alpha[3];
    vsop87s_t *groups[3];
};

class OrbitVSOP87 : public OrbitEphemeris
{
public:
    OrbitVSOP87(Celestial &cbody, vsop87p_t &series);
    virtual ~OrbitVSOP87();

    static OrbitEphemeris *create(Celestial &cbody, cstr_t &name);

protected:
    void setSeries(char series);
    void init();
    
    void getEphemeris(double mjd, double *res);

    vsop87p_t &series;

    double a0;      // semi-major axis [AU]

    int16_t fmtFlags = 0;

private:
    char sid;

};