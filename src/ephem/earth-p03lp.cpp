// p03lp.cpp - Long-precision Earth precession rotational model package
//
// Using body ecliptic-plabe frame
//
// Author:  Tim Stark
// Date:    Apr 23, 2022

#include "main/core.h"
#include "ephem/rotation.h"
#include "universe/astro.h"

// ******** Earth Rotation Model ********

#define P03LP_PRECESSION_CENTURIES 5000.0

struct EclipticPrecessionTerm
{
	double	Pc;
	double	Qc;
	double	Ps;
	double	Qs;
	double	period;
};

struct PrecessionTerm
{
	double	Pc;
	double	Ec;
	double	Ps;
	double	Es;
	double	period;
};


static EclipticPrecessionTerm eclipticPrecessionTerms[] =
{
	// 	CPa				CQa				SPa			SQa		period
    {   486.230527, 2559.065245, -2578.462809,   485.116645, 2308.98 },
    {  -963.825784,  247.582718,  -237.405076,  -971.375498, 1831.25 },
    { -1868.737098, -957.399054,  1007.593090, -1930.464338,  687.52 },
    { -1589.172175,  493.021354,  -423.035168, -1634.905683,  729.97 },
    {   429.442489, -328.301413,   337.266785,   429.594383,  492.21 },
    { -2244.742029, -339.969833,   221.240093, -2131.745072,  708.13 },
};

static PrecessionTerm precessionTerms[] =
{
	//   cPa			cEa				sPa			sEa			period
    { -6180.062400,   807.904635, -2434.845716, -2056.455197,  409.90 },
    { -2721.869299,  -177.959383,   538.034071,  -912.727303,  396.15 },
    {  1460.746498,   371.942696, -1245.689351,   447.710000,  536.91 },
    { -1838.488899,  -176.029134,   529.220775,  -611.297411,  402.90 },
    {   949.518077,   -89.154030,   277.195375,   315.900626,  417.15 },
    {    32.701460,  -336.048179,   945.979710,    12.390157,  288.92 },
    {   598.054819,   -17.415730,  -955.163661,   -15.922155, 4042.97 },
    {  -293.145284,   -28.084479,    93.894079,  -102.870153,  304.90 },
    {    66.354942,    21.456146,     0.671968,    24.123484,  281.46 },
    {    18.894136,    30.917011,  -184.663935,     2.512708,  204.38 },
};

void computeEclipticPrecission(double T, double &pa, double &qa)
{
	double T2 = T * T;
	double T3 = T * T * T;

	pa =  5750.804069 + 0.1948311 * T - 0.00016739 * T2 - 4.8e-8 * T3;
	qa = -1673.999018 + 0.3474459 * T + 0.00011243 * T2 - 6.4e-8 * T3;

	int nTerms = sizeof(eclipticPrecessionTerms) / sizeof(eclipticPrecessionTerms[0]);
	for (int idx = 0; idx < nTerms; idx++) {
		const EclipticPrecessionTerm &p = eclipticPrecessionTerms[idx];

		double theta = 2.0 * pi * T / p.period;
		double s = sin(theta);
		double c = cos(theta);

		pa += (p.Pc * c) + (p.Ps * s);
		qa += (p.Qc * c) + (p.Qs * s);
	}
}

void computePrecissionObliquity(double T, double &pa, double &ea)
{
	double T2 = T * T;
	double T3 = T * T * T;

	pa =  7907.295950 + 5044.374034 * T - 0.00713473 * T2 + 6e-9 * T3;
	ea =  83973.876448 - 0.0425899 * T - 0.0000113 * T2;

	int nTerms = sizeof(precessionTerms) / sizeof(precessionTerms[0]);
	for (int idx = 0; idx < nTerms; idx++) {
		const PrecessionTerm &p = precessionTerms[idx];

		double theta = 2.0 * pi * T / p.period;
		double s = sin(theta);
		double c = cos(theta);

		pa += (p.Pc * c) + (p.Ps * s);
		ea += (p.Ec * c) + (p.Es * s);
	}
}

quatd_t EarthRotation::computeSpin(double tjd) const
{
    double t = tjd - astro::J2000;

    double theta = (pi*2.0) * (t * 24.0 / 23.9344694 - 79.853 / 360.0);

	// fmt::printf("Yes, spin here - %lf\n", glm::degrees(theta));

    return yRotate(-theta);
}

quatd_t EarthRotation::computeEquatorRotation(double tjd) const
{
    double T = (tjd - astro::J2000) / 36525.0;

    if (T < -P03LP_PRECESSION_CENTURIES)
        T =- P03LP_PRECESSION_CENTURIES;
    else if (T > P03LP_PRECESSION_CENTURIES)
        T = P03LP_PRECESSION_CENTURIES;

    double opa, osa, ppa, pqa;

	computePrecissionObliquity(T, opa, osa);
	computeEclipticPrecission(T, ppa, pqa);

	double obliquity  = glm::radians(osa / 3600);
	double precession = glm::radians(ppa / 3600);

	double P = ppa * 2.0 * pi / 129600;
	double Q = pqa * 2.0 * pi / 129600;
	double piA = asin(sqrt(P*P + Q*Q));
	double PiA = atan2(P, Q);

	quatd_t RPi = zRotate(PiA);
	quatd_t rpi = xRotate(piA);
	quatd_t eclRotation = glm::conjugate(RPi) * rpi * RPi;

	quatd_t q = xRotate(obliquity) * zRotate(-precession) * glm::conjugate(eclRotation);

	// fmt::printf("Yes, here equator rotation\n");

	return xRotate(pi / 2.0) * q * xRotate(-pi / 2.0);
}