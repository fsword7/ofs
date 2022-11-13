// elp82.h - Moon ELP82 epheremis data package
//
// Author:  Tim Stark
// Date:    Nov 4, 2022

#pragma once

class OrbitELP82
{
public:
    OrbitELP82() = default;
    ~OrbitELP82() = default;

    void getEphemeris(double mjd, double *res);

protected:
    void init();
    void load(cstr_t &name);

private:
    double delnu, dele, delg, delnp, delep;
    double p1, p2, p3, p4, p5, q1, q2, q3, q4, q5;
    double cpi, cpi2, pis2, rad, deg, c1, c2, ath, a0, am, alpha, dtasm, precess;
    double mjd2000, sc, pscale, vscale;
    double w[3][5], p[8][2], eart[5], peri[5], del[4][5], zeta[2];
    int    nterm[3][12], nrang[3][12];

};


