// elp82.h - Moon ELP82 epheremis data package
//
// Author:  Tim Stark
// Date:    Nov 4, 2022

#pragma once

class CelestialBody;

struct elp82main_t
{
    int ilu[4];
    double coef[7];
};

struct elp82group_t
{
    elp82main_t *main;
    int nterms;
};

typedef double SEQ3[3];
typedef double SEQ6[6];
class OrbitELP82 : public OrbitEphemeris
{
public:
    OrbitELP82(CelestialBody &cbody);
    ~OrbitELP82() = default;

    void getEphemeris(double mjd, double *res);

    virtual uint16_t getOrbitData(double mjd, uint16_t req, double *res) = 0;

    static OrbitEphemeris *create(CelestialBody &cbody, cstr_t &name);

protected:
    void init();
    void initData(double prec);

private:
    double delnu, dele, delg, delnp, delep;
    double p1, p2, p3, p4, p5, q1, q2, q3, q4, q5;
    double cpi, cpi2, pis2, rad, deg, c1, c2, ath, a0, am, alpha, dtasm, precess;
    double mjd2000, sc, pscale, vscale;
    double w[3][5], p[8][2], eart[5], peri[5], del[4][5], zeta[2];
    int    nterm[3][12], nrang[3][12];

    SEQ6 *pc[3]  = {};
    SEQ3 *per[3] = {};

    const double def_prec = 1e-5;
    double cur_prec = -1.0;
};


