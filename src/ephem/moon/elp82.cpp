// elp82.cpp - Moon ELP82 epheremis data package
//
// Author:  Tim Stark
// Date:    Nov 4, 2022

#include "main/core.h"
#include "api/ofsapi.h"
#include "api/celbody.h"
#include "ephem/moon/elp82.h"

void OrbitELP82::load(cstr_t &name)
{

}

void OrbitELP82::init()
{
    // Constant initializations
    cpi     = 3.141592653589793;
    cpi2    = 2.0*cpi;
    pis2    = cpi/2.0;
    rad     = 648000.0/cpi;
    deg     = cpi/180.0;
    c1      = 60.0;
    c2      = 3600.0;
    ath     = 384747.9806743165;
    a0      = 384747.9806448954;
    am      = 0.074801329518;
    alpha   = 0.002571881335;
    dtasm   = 2.0*alpha/(3.0*am);
    precess = 5029.0966/rad;

    mjd2000 = 51544.5; // mjd->dj2000 offset
    sc      = 36525.0;
	pscale  = 1e3;
	vscale  = 1e3/(86400.0*sc);

    // Lunar arguments
    w[0][0] = (218.0+18.0/c1+59.95571/c2)*deg;
	w[1][0] = (83.0+21.0/c1+11.67475/c2)*deg;
	w[2][0] = (125.0+2.0/c1+40.39816/c2)*deg;
	eart[0] = (100.0+27.0/c1+59.22059/c2)*deg;
	peri[0] = (102.0+56.0/c1+14.42753/c2)*deg;
	w[0][1] = 1732559343.73604/rad;
	w[1][1] = 14643420.2632/rad;
	w[2][1] = -6967919.3622/rad;
	eart[1] = 129597742.2758/rad;
	peri[1] = 1161.2283/rad;
	w[0][2] = -5.8883/rad;
	w[1][2] = -38.2776/rad;
	w[2][2] = 6.3622/rad;
	eart[2] = -0.0202/rad;
	peri[2] = 0.5327/rad;
	w[0][3] = 0.6604e-2/rad;
	w[1][3] = -0.45047e-1/rad;
	w[2][3] = 0.7625e-2/rad;
	eart[3] = 0.9e-5/rad;
	peri[3] = -0.138e-3/rad;
	w[0][4] = -0.3169e-4/rad;
	w[1][4] = 0.21301e-3/rad;
	w[2][4] = -0.3586e-4/rad;
	eart[4] = 0.15e-6/rad;
	peri[4] = 0.0;

	// Planetary arguments
	p[0][0] = (252.0+15.0/c1+3.25986/c2)*deg;
	p[1][0] = (181.0+58.0/c1+47.28305/c2)*deg;
	p[2][0] = eart[0];
	p[3][0] = (355.0+25.0/c1+59.78866/c2)*deg;
	p[4][0] = (34.0+21.0/c1+5.34212/c2)*deg;
	p[5][0] = (50.0+4.0/c1+38.89694/c2)*deg;
	p[6][0] = (314.0+3.0/c1+18.01841/c2)*deg;
	p[7][0] = (304.0+20.0/c1+55.19575/c2)*deg;
	p[0][1] = 538101628.68898/rad;
	p[1][1] = 210664136.43355/rad;
	p[2][1] = eart[1];
	p[3][1] = 68905077.59284/rad;
	p[4][1] = 10925660.42861/rad;
	p[5][1] = 4399609.65932/rad;
	p[6][1] = 1542481.19393/rad;
	p[7][1] = 786550.32074/rad;

    // Corrections of the constants (fit to DE200/LE200)
	delnu = +0.55604/rad/w[0][1];
	dele  = +0.01789/rad;
	delg  = -0.08066/rad;
	delnp = -0.06424/rad/w[0][1];
	delep = -0.12879/rad;

	// Delaunay's arguments
	for (int i = 0; i < 5; i++) {
		del[0][i] = w[0][i] - eart[i];
		del[3][i] = w[0][i] - w[2][i];
		del[2][i] = w[0][i] - w[1][i];
		del[1][i] = eart[i] - peri[i];
	}
	del[0][0] = del[0][0] + cpi;
	zeta[0]   = w[0][0];
	zeta[1]   = w[0][1] + precess;

	// Precession matrix
	p1 =  0.10180391e-4;
	p2 =  0.47020439e-6;
	p3 = -0.5417367e-9;
	p4 = -0.2507948e-11;
	p5 =  0.463486e-14;
	q1 = -0.113469002e-3;
	q2 =  0.12372674e-6;
	q3 =  0.1265417e-8;
	q4 = -0.1371808e-11;
	q5 = -0.320334e-14;
}

void OrbitELP82::getEphemeris(double mjd, double *res)
{
	int k, iv, nt;
	double t[5];
	double x, y, x1, x2, x3, pw, qw, ra, pwqw, pw2, qw2;
	double x_dot, y_dot, x1_dot, x2_dot, x3_dot, pw_dot, qw_dot;
	double ra_dot, pwqw_dot, pw2_dot, qw2_dot;
	double cosr0, sinr0, cosr1, sinr1;

	// Initialisation

	// if (need_terms) ELP82_read (def_prec);

	// substitution of time

	t[0] = 1.0;
    t[1] = (mjd-mjd2000)/sc;
    t[2] = t[1]*t[1];
    t[3] = t[2]*t[1];
    t[4] = t[3]*t[1];

	// for (iv = 0; iv < 3; iv++) {
	// 	res[iv] = res[iv+3] = 0.0;
	// 	// SEQ6 *pciv = pc[iv];

	// 	// main sequence (itab=0)
	// 	for (nt = 0; nt < nterm[iv][0]; nt++) {
	// 		x = pciv[nt][0];     x_dot = 0.0;
	// 		y = pciv[nt][1];     y_dot = 0.0;
	// 		for (k = 1; k <= 4; k++) {
	// 			y     += pciv[nt][k+1] * t[k];
	// 			y_dot += pciv[nt][k+1] * t[k-1] * k;
	// 		}
	// 		res[iv]   += x*sin(y);
	// 		res[iv+3] += x_dot*sin(y) + x*cos(y)*y_dot;
	// 	}

	// }

	// Change of coordinates
	res[0] = res[0]/rad + w[0][0] + w[0][1]*t[1] + w[0][2]*t[2] + w[0][3]*t[3] + 
		              w[0][4]*t[4];
	res[3] = res[3]/rad + w[0][1] + 2*w[0][2]*t[1] + 3*w[0][3]*t[2] + 4*w[0][4]*t[3];
	res[1] = res[1]/rad;
	res[4] = res[4]/rad;
	res[2] = res[2]*a0/ath;
	res[5] = res[5]*a0/ath;
	cosr0 = cos(res[0]), sinr0 = sin(res[0]);
	cosr1 = cos(res[1]), sinr1 = sin(res[1]);
	x1       = res[2]*cosr1;
	x1_dot   = res[5]*cosr1 - res[2]*sinr1*res[4];
	x2       = x1*sinr0;
	x2_dot   = x1_dot*sinr0 + x1*cosr0*res[3];
	x1_dot   = x1_dot*cosr0 - x1*sinr0*res[3];
	x1       = x1*cosr0;
	x3       = res[2]*sinr1;
	x3_dot   = res[5]*sinr1 + res[2]*cosr1*res[4];
	pw       = (p1+p2*t[1]+p3*t[2]+p4*t[3]+p5*t[4])*t[1];
	pw_dot   = p1 + 2*p2*t[1] + 3*p3*t[2] + 4*p4*t[3] + 5*p5*t[4];
	qw       = (q1+q2*t[1]+q3*t[2]+q4*t[3]+q5*t[4])*t[1];
	qw_dot   = q1 + 2*q2*t[1] + 3*q3*t[2] + 4*q4*t[3] + 5*q5*t[4];
	ra       = 2.0*sqrt(1-pw*pw-qw*qw);
	ra_dot   = -4.0*(pw+qw)/ra;
	pwqw     = 2.0*pw*qw;
	pwqw_dot = 2.0*(pw_dot*qw + pw*qw_dot);
	pw2      = 1-2.0*pw*pw;
	pw2_dot  = -4.0*pw;
	qw2      = 1-2.0*qw*qw;
	qw2_dot  = -4.0*qw;
	pw       = pw*ra;
	pw_dot   = pw_dot*ra + pw*ra_dot;
	qw       = qw*ra;
	qw_dot   = qw_dot*ra + qw*ra_dot;

    // End of ELP82 calculation
    // swap y and z compoments for left-handed coordinates
    // res[1] <-> res[2], res[4] <-> res[5]

    // Position vector
	res[0] = pw2*x1+pwqw*x2+pw*x3;
    res[1] = -pw*x1+qw*x2+(pw2+qw2-1)*x3;
	res[2] = pwqw*x1+qw2*x2-qw*x3;

    // Velocity vector
	res[3] = pw2_dot*x1 + pw2*x1_dot + pwqw_dot*x2 + pwqw*x2_dot + pw_dot*x3 + pw*x3_dot;
	res[4] = -pw_dot*x1 - pw*x1_dot + qw_dot*x2 + qw*x2_dot + (pw2_dot+qw2_dot)*x3 + (pw2+qw2-1)*x3_dot;
	res[5] = pwqw_dot*x1 + pwqw*x1_dot + qw2_dot*x2 + qw2*x2_dot - qw_dot*x3 - qw*x3_dot;

	// convert to m and m/s
	res[0] *= pscale;
	res[1] *= pscale;
	res[2] *= pscale;

	res[3] *= vscale;
	res[4] *= vscale;
	res[5] *= vscale;
}